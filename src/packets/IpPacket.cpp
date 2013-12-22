//
// Copyright (C) 2008-2011 Institute of Telematics, Karlsruhe Institute of Technology (KIT)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "IpPacket.h"
#include "TcpPacket.h"
#include "UdpPacket.h"

AnonPrimitive*	IpPacket::anonTos		= NULL;
AnonPrimitive*	IpPacket::anonIdentifier	= NULL;
AnonPrimitive*	IpPacket::anonFlags		= NULL;
AnonPrimitive*	IpPacket::anonFragoffset	= NULL;
AnonPrimitive*	IpPacket::anonTtl		= NULL;
AnonPrimitive*	IpPacket::anonSourceip		= NULL;
AnonPrimitive*	IpPacket::anonDestip		= NULL;
AnonPrimitive*	IpPacket::anonOptions		= NULL;

IpPacket::IpPacket(void)
{
	options.buf	= NULL;
	options.size	= 0;
	memset		(&header, 0, sizeof (IP_HEADER));
	protocol	= Packet::PROTO_IP;
}

IpPacket::~IpPacket(void)
{
	options.destroy ();
}

bool IpPacket::parsePacket() 
{
	memcpy (&header, buffer, sizeof (IP_HEADER));

	//
	// check for correct checksum
	//

	if (Configuration::instance()->getSetBadChecksumsToBad ()) {
		uint16_t chksum	= header.checksum;
		header.checksum		= 0;
		this->checksumgood	= (checksum (&header) == chksum);
		header.checksum		= chksum;
	}
		
	//
	// correct some endianess stuff
	//

	header.totallen		= swap16(header.totallen);
	header.identification	= swap16(header.identification);
	header.flags_fragoffset	= swap16(header.flags_fragoffset);
	header.checksum		= swap16(header.checksum);	

	//
	// get the next transport layer protocol
	//

	nextProtocol = IpPacket::getTransportProtocol (header.protocol);

	//
	// check for correct header values to make sure the packet is not corrupt
	// if is is corrupt we will stop the parsing here and return
	//

	if (! ( IP_HEADER_NO_OPTIONS_LEN <= getHeaderlength () && 
                getHeaderlength() <= getTotallen() )) {
		
		layersize	= getSize ();
		nextProtocol	= Packet::PROTO_NONE;
		options.size	= 0;
		options.buf	= NULL;
		return true;
	}

	//
	// get the options, if any are provided
	// 

	options.size	= getHeaderlength () - IP_HEADER_NO_OPTIONS_LEN;
	options.buf	= NULL;

	if (options.size > 0 && getSize () >= IP_HEADER_NO_OPTIONS_LEN + options.size) {
		options.buf = (uint8_t*) malloc (options.size);
		memcpy (options.buf, buffer + IP_HEADER_NO_OPTIONS_LEN, options.size);
	} else
		options.size = 0;

	layersize = sizeof (IP_HEADER) + options.size;

	//
	// remove the ethernet padding. Otherwise
	// it will be used as payload.
	// the ethernet class will add it again
	// 
	
	if (getSize () > getTotallen ())
		this->size = getTotallen ();

	return true;
}

uint16_t IpPacket::checksum (PIP_HEADER ipheader)
{
	return Packet::checksum ((uint8_t*)ipheader, sizeof (IP_HEADER));
}

uint8_t IpPacket::getVersion ()
{
	return IP_HEADER_VERSION (header.version_len);
}

uint8_t IpPacket::getHeaderlength ()
{
	return IP_HEADER_LEN (header.version_len);
}

uint8_t IpPacket::getTos ()
{
	return header.typeofservice;
}

uint16_t IpPacket::getIdentification ()
{
	return header.identification;
}

uint16_t IpPacket::getFlags ()
{
	return IP_HEADER_FLAGS (header.flags_fragoffset);
}

uint16_t IpPacket::getFragoffset ()
{
	return IP_HEADER_FRAGOFFSET (header.flags_fragoffset);
}

uint8_t IpPacket::getTtl ()
{
	return header.ttl;
}

uint8_t IpPacket::getProtocol ()
{
	return header.protocol;
}

uint16_t IpPacket::getChecksum ()
{
	return header.checksum;
}

IP_ADDR IpPacket::getSourceip ()
{	
	IP_ADDR ret = {{0}};
	memcpy (&ret, &header.sourceip, IP_ADDR_LEN);
	return ret;
}

IP_ADDR	IpPacket::getDestip ()
{
	IP_ADDR ret = {{0}};
	memcpy (&ret, &header.destip, IP_ADDR_LEN);
	return ret;
}

void IpPacket::setVersion (uint8_t ver)
{
	header.version_len = ((ver & 0xF) << 4) | (getHeaderlength() / 4);
}

void IpPacket::setHeaderlength (uint8_t len)
{
	header.version_len = (getVersion() << 4) | ((uint8_t)((len/4) & 0xF));
}

void IpPacket::setTos (uint8_t tos)
{
	header.typeofservice = tos;
}

void IpPacket::setIdentification (uint16_t ident)
{
	header.identification = ident;
}

void IpPacket::setFlags (uint16_t	flgs)
{
	header.flags_fragoffset = ((uint16_t)(flgs & 0x07) << 13) | getFragoffset();
}

void IpPacket::setFragoffset (uint16_t fragoff)
{
	header.flags_fragoffset = ((getFlags() & 0x07) << 13) | (0xD & fragoff);
}

void IpPacket::setTtl (uint8_t tl)
{
	header.ttl = tl;
}

void IpPacket::setProtocol (uint8_t proto)
{
	header.protocol = proto;
}

void IpPacket::setChecksum (uint16_t checks)
{
	header.checksum = checks;
}

void IpPacket::setSourceip (IP_ADDR sip)
{
	memcpy (&header.sourceip, &sip, IP_ADDR_LEN);
}

void IpPacket::setDestip (IP_ADDR dip)
{
	memcpy (&header.destip, &dip, IP_ADDR_LEN);
}	

uint16_t IpPacket::getTotallen ()
{
	return header.totallen;
}

void IpPacket::setTotallen (uint16_t totlen)
{
	header.totallen = totlen;
}

void IpPacket::assemblePacket()
{
	if (nextPacket != NULL) {
		
		// TCP and UDP packets need the source
		// and dest IP addresses to calc their 
		// checksums. So we pass it over here
		// where it is already transformed.

		if (nextPacket->getProtocol() == Packet::PROTO_TCP) {
			((TcpPacket*)nextPacket)->setIpAddresses (getSourceip(), getDestip());
		} else if (nextPacket->getProtocol() == Packet::PROTO_UDP) {
			((UdpPacket*)nextPacket)->setIpAddresses (getSourceip(), getDestip());
		}

		nextPacket->assemblePacket ();
	}

	int thissize = sizeof (IP_HEADER) + options.size;
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	// 
	// adjust length
	//

	setHeaderlength	(thissize);
	setTotallen	(thissize + nextsize);

	//
	// correct endianess for copying
	//

	header.totallen		= swap16(header.totallen	);
	header.identification	= swap16(header.identification	);
	header.flags_fragoffset	= swap16(header.flags_fragoffset);
	header.checksum		= swap16(header.checksum	);

	//
	// calc checksum or set to random if the value was incorrect
	//

	setChecksum (0);

	if (Configuration::instance()->getReCalcChecksums ()) {

		if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood) 
			setChecksum (RandomNumberGenerator::generate ());
		else
			setChecksum (checksum (&header));

	} // if (Configuration::instance()->getReCalcChecksums ()) 

	//
	// copy into out buffer
	//

	memcpy (buffer, &header, sizeof (IP_HEADER));
	memcpy (buffer + sizeof (IP_HEADER), options.buf,options.size);
	
	//
	// convert endianess back
	//

	header.totallen		= swap16(header.totallen	);
	header.identification	= swap16(header.identification	);
	header.flags_fragoffset	= swap16(header.flags_fragoffset);
	header.checksum		= swap16(header.checksum	);	
}

void IpPacket::setOptions (uint8_t* buf, int len)
{
	options.buf	= (uint8_t*) realloc (options.buf, len);
	options.size	= len;
	memcpy (options.buf, buf, len);
}

Packet::PAYLOAD_BUFFER IpPacket::getOptions()
{
	PAYLOAD_BUFFER retbuf;
	
	retbuf.buf = (uint8_t*) malloc (options.size);
	retbuf.size = options.size;

	memcpy (retbuf.buf, options.buf, options.size);

	return retbuf;
}

string IpPacket::toString ()
{
	ostringstream out;
	Packet::PAYLOAD_BUFFER opts = getOptions ();

	out << "IP packet"		<< std::endl
		<< "\tversion: \t"	<< (int)getVersion()		<< std::endl
		<< "\tlength: \t"	<< (int)getHeaderlength()	<< " bytes" << std::endl
		<< "\ttos: \t\t0x"	<< std::hex << std::setw(2)	<< std::setfill ('0')<< (int)getTos () << std::endl << std::dec
		<< "\ttotal length: \t" << getTotallen() << " bytes"	<< std::endl
		<< "\tident: \t\t0x"	<< std::hex << std::setw(4)	<< std::setfill ('0') << getIdentification () << std::endl << std::dec
		<< "\tflags: \t\t0x"	<< std::hex << std::setw(2)	<< std::setfill ('0') << getFlags () << std::endl << std::dec
		<< "\tfragoff: \t0x"	<< std::hex << std::setw(4)	<< std::setfill ('0') << getFragoffset () << std::endl << std::dec
		<< "\tttl: \t\t"	<< (int)getTtl()		<< std::endl
		<< "\tprotocol: \t0x"	<< std::hex << std::setw(2)	<< std::setfill ('0') << (int)getProtocol () << std::endl << std::dec
		<< "\tchecksum: \t0x"	<< std::hex << std::setw(4)	<< std::setfill ('0') << getChecksum () << std::endl << std::dec
		<< "\tsource ip: \t"	<< getSourceip().toString()	<< std::endl
		<< "\tdest ip: \t"	<< getDestip().toString()	<< std::endl
		<< "\toptions len: \t"	<< opts.size			<< std::endl
		<< "\toptions: \t"	<< opts.toString();
	
	opts.destroy ();
	return out.str ();
}

uint32_t IpPacket::getMinProtocolSize ()
{
	return sizeof (IP_HEADER);
}

Packet::PROTOCOL IpPacket::getTransportProtocol (uint8_t proto)
{
	Packet::PROTOCOL nextproto = Packet::PROTO_DATA_PAYLOAD;

	switch (proto) {
		case IPTYPE_TCP:	nextproto = Packet::PROTO_TCP;			break;
		case IPTYPE_UDP:	nextproto = Packet::PROTO_UDP;			break;
		case IPTYPE_ICMP:	nextproto = Packet::PROTO_ICMP;			break;
		case IPTYPE_IP:		nextproto = Packet::PROTO_IP;			break;
		case IPTYPE_IPV6:	nextproto = Packet::PROTO_IPV6;			break;
		default:		nextproto = Packet::PROTO_DATA_PAYLOAD;		break;
	}

	return nextproto;
}
