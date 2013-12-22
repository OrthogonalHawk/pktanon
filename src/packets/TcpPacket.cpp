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

#include "TcpPacket.h"

AnonPrimitive* TcpPacket::anonSourceport	= NULL;
AnonPrimitive* TcpPacket::anonDestport		= NULL;
AnonPrimitive* TcpPacket::anonSeqnum		= NULL;
AnonPrimitive* TcpPacket::anonAcknum		= NULL;
AnonPrimitive* TcpPacket::anonFlags		= NULL;
AnonPrimitive* TcpPacket::anonWindowsize	= NULL;
AnonPrimitive* TcpPacket::anonUrgentpnt		= NULL;
AnonPrimitive* TcpPacket::anonOptions		= NULL;

TcpPacket::TcpPacket(void)
{
	options.buf	= NULL;
	options.size	= 0;
	memset		(&header, 0, sizeof (TCP_HEADER));
	protocol	= Packet::PROTO_TCP;
}

TcpPacket::~TcpPacket(void)
{
	options.destroy ();
}

bool TcpPacket::parsePacket()
{
	memcpy (&header, buffer, sizeof (TCP_HEADER)); 

	//
	// check for correct header values to make sure the packet is not corrupt
	// if is is corrupt we will stop the parsing here and return
	//

	header.headerlen_flags = swap16	(header.headerlen_flags);

	if (! (	TCP_HEADER_NO_OPTIONS_LEN <= getHeaderlength()  && 
		getHeaderlength() <= getSize() )) {
		
		layersize	= getSize ();
		nextProtocol	= Packet::PROTO_NONE;
		options.size	= 0;
		options.buf	= NULL;
		return true;
	}

	//
	// get the options, if any are provided
	// 

	options.size	= getHeaderlength() - TCP_HEADER_NO_OPTIONS_LEN;
	options.buf	= NULL;
	

	if (options.size > 0 && getSize() >= TCP_HEADER_NO_OPTIONS_LEN + options.size) {
		options.buf = (uint8_t*) malloc (options.size);
		memcpy (options.buf, buffer + TCP_HEADER_NO_OPTIONS_LEN, options.size);
	} else
		options.size = 0;

	nextProtocol	= Packet::PROTO_DATA_PAYLOAD;
	layersize	= sizeof (TCP_HEADER) + options.size;

	header.headerlen_flags = swap16	(header.headerlen_flags);

	//
	// check for correct checksum
	//

	if (Configuration::instance()->getSetBadChecksumsToBad ()) {

		uint16_t chksum	= header.checksum;
		header.checksum		= 0;

		void* pipsource	= (ipProtocol == Packet::PROTO_IP ? (void*)&sourceip	: (void*)&sourceip6);
		void* pipdest	= (ipProtocol == Packet::PROTO_IP ? (void*)&destip		: (void*)&destip6	);

		this->checksumgood = (checksum (&header, 
						pipsource, 
						pipdest, 
						ipProtocol,
						options.buf, 
						options.size, 
						getBuffer	() + layersize, 
						getSize		() - layersize
						) == chksum);
		header.checksum			= chksum;

	} // if (Configuration::instance()->getSetBadChecksumsToBad ()) 

	//
	// swap some values
	//

	header.acknum		= swap32(header.acknum		);
	header.checksum		= swap16(header.checksum	);
	header.destport		= swap16(header.destport	);
	header.headerlen_flags	= swap16(header.headerlen_flags	);
	header.sequencenum	= swap32(header.sequencenum	);	
	header.sourceport	= swap16(header.sourceport	);
	header.urgentpointer	= swap16(header.urgentpointer	);
	header.windowsize	= swap16(header.windowsize	);

	return true;
}

uint16_t TcpPacket::getSourceport()
{
	return header.sourceport;
}
		
uint16_t TcpPacket::getDestport()
{
	return header.destport;
}
		
uint32_t TcpPacket::getSequencenum()
{
	return header.sequencenum;
}
		
uint32_t TcpPacket::getAcknum()
{
	return header.acknum;
}
		
uint16_t TcpPacket::getHeaderlength()
{
	return TCP_HEADER_LEN (header.headerlen_flags);
}
		
uint16_t TcpPacket::getFlags()
{
	return TCP_HEADER_FLAGS (header.headerlen_flags);
}
		
uint16_t TcpPacket::getWindowsize()
{
	return header.windowsize;
}
			
uint16_t TcpPacket::getChecksum()
{
	return header.checksum;
}
		
uint16_t TcpPacket::getUrgentpointer()
{
	return header.urgentpointer;
}

void TcpPacket::setSourceport(uint16_t sourcep)
{
	header.sourceport = sourcep;
}

void TcpPacket::setDestport(uint16_t destp)
{
	header.destport = destp;
}
		
void TcpPacket::setSequencenum(uint32_t seqnum)
{
	header.sequencenum = seqnum;
}
		
void TcpPacket::setAcknum(uint32_t aknum)
{
	header.acknum = aknum;
}
		
void TcpPacket::setHeaderlength(uint16_t hdlen)
{
	header.headerlen_flags = (((hdlen/4) & 0xF) << 12) | (getFlags() & 0x3F);
}
		
void TcpPacket::setFlags(uint16_t flgs)
{
	header.headerlen_flags = (((getHeaderlength()/4)& 0xF) << 12) | (flgs & 0x3F);
}
	
void TcpPacket::setWindowsize(uint16_t wsize)
{
	header.windowsize = wsize;
}
			
void TcpPacket::setChecksum(uint16_t cksum)
{
	header.checksum = cksum;
}
		
void TcpPacket::setUrgentpointer(uint16_t	urgpnt)
{
	header.urgentpointer = urgpnt;
}
	
void TcpPacket::assemblePacket()
{
	if (nextPacket != NULL)
		nextPacket->assemblePacket ();

	int thissize = sizeof (TCP_HEADER) + options.size;
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	//
	// correct header length
	//

	setHeaderlength (thissize);
	
	//
	// correct endianess for output buffer
	//

	header.acknum		= swap32(header.acknum		);
	header.checksum		= swap16(header.checksum	);
	header.destport		= swap16(header.destport	);
	header.headerlen_flags	= swap16(header.headerlen_flags	);
	header.sequencenum	= swap32(header.sequencenum	);	
	header.sourceport	= swap16(header.sourceport	);
	header.urgentpointer	= swap16(header.urgentpointer	);
	header.windowsize	= swap16(header.windowsize	);

	//
	// calculate new checksum
	// therefore we need the PayloadPacket
	//

	setChecksum (0);

	if (Configuration::instance()->getReCalcChecksums ()) {

		if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood) {

			setChecksum (RandomNumberGenerator::generate ());

		} else {

			uint8_t*	data	= NULL;
			uint32_t	datalen = 0;

			if (nextProtocol != Packet::PROTO_NONE && nextPacket != NULL) {
				data	= nextPacket->getBuffer ();
				datalen = nextPacket->getSize	();
			}

			void* pipsource	= (ipProtocol == Packet::PROTO_IP ? (void*)&sourceip	: (void*)&sourceip6);
			void* pipdest	= (ipProtocol == Packet::PROTO_IP ? (void*)&destip		: (void*)&destip6	);

			setChecksum		(checksum (&header, pipsource, pipdest, ipProtocol, options.buf, options.size, data, datalen));

		} // if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood) {

	} // if (Configuration::instance()->getReCalcChecksums ()) 

	//
	// copy into out buffer
	// header, options, data
	//

	memcpy (buffer, &header, sizeof (TCP_HEADER));
	memcpy (buffer + sizeof (TCP_HEADER), options.buf, options.size);

	//
	// convert endianess back
	//

	header.acknum		= swap32(header.acknum		);
	header.checksum		= swap16(header.checksum	);
	header.destport		= swap16(header.destport	);
	header.headerlen_flags	= swap16(header.headerlen_flags	);
	header.sequencenum	= swap32(header.sequencenum	);	
	header.sourceport	= swap16(header.sourceport	);
	header.urgentpointer	= swap16(header.urgentpointer	);
	header.windowsize	= swap16(header.windowsize	);
}

uint16_t TcpPacket::checksum (PTCP_HEADER header, void* sourceip, void* destip, Packet::PROTOCOL ipproto,uint8_t* options, int optionslen, uint8_t* data, int datalen)
{
	//
	// create a pseudoheader
	// http://www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm
	//
	
	if (optionslen	== 0) options	= NULL;
	if (datalen	== 0) data	= NULL;

	int tcplen = sizeof (TCP_HEADER) + optionslen + datalen;

	//
	// create the approriate pseudo header (with IPv4 or IPv6 addresses)
	//

	PTCP_PSEUDO_HEADER pseudoheader = NULL;
	uint32_t pseudolength = 0;

	if (ipproto == Packet::PROTO_IP)	{
		pseudoheader = new TCP_IP4_PSEUDO_HEADER ();
		pseudolength = sizeof (TCP_IP4_PSEUDO_HEADER);
		
		memcpy (&((PTCP_IP4_PSEUDO_HEADER)pseudoheader)->sourceip, sourceip, IP_ADDR_LEN);
		memcpy (&((PTCP_IP4_PSEUDO_HEADER)pseudoheader)->destip,   destip,   IP_ADDR_LEN);

	} else {
		pseudoheader = new TCP_IP6_PSEUDO_HEADER ();
		pseudolength = sizeof (TCP_IP6_PSEUDO_HEADER);

		memcpy (&((PTCP_IP6_PSEUDO_HEADER)pseudoheader)->sourceip, sourceip, IPV6_ADDR_LEN);
		memcpy (&((PTCP_IP6_PSEUDO_HEADER)pseudoheader)->destip,   destip,   IPV6_ADDR_LEN);
	}

	pseudoheader->reserved	= 0;
	pseudoheader->protocol	= IPTYPE_TCP;
	pseudoheader->tcplength	= swap16 (tcplen);

	//
	// create a data buffer
	//

	int buflen = pseudolength + tcplen;
	uint8_t* buf = (uint8_t*) malloc (buflen);

	memset (buf, 0, buflen);

	//
	// copy pseudoheader header, options, data into buffer
	// 

	memcpy (buf, pseudoheader, pseudolength);
	memcpy (buf + pseudolength, header, sizeof (TCP_HEADER));
	memcpy (buf + pseudolength + sizeof (TCP_HEADER), options, optionslen);
	memcpy (buf + pseudolength + sizeof (TCP_HEADER) + optionslen, data, datalen);

	//
	// create the checksum
	//

	uint16_t sum = Packet::checksum (buf, buflen);

	free (buf);
	delete pseudoheader;

	return sum;
}

void TcpPacket::setIpAddresses (IP_ADDR source, IP_ADDR dest)
{
	sourceip	= source;
	destip		= dest;
	ipProtocol	= Packet::PROTO_IP;
}

void TcpPacket::setIpAddresses (IPV6_ADDR source, IPV6_ADDR dest)
{
	sourceip6	= source;
	destip6		= dest;
	ipProtocol	= Packet::PROTO_IPV6;
}

void TcpPacket::setOptions (uint8_t* buf, int len)
{
	options.buf = (uint8_t*) realloc (options.buf, len);
	options.size = len;
	memcpy (options.buf, buf, len);
}

Packet::PAYLOAD_BUFFER TcpPacket::getOptions	()
{
	PAYLOAD_BUFFER retbuf;
	
	retbuf.buf = (uint8_t*) malloc (options.size);
	retbuf.size = options.size;

	memcpy (retbuf.buf, options.buf, options.size);

	return retbuf;
}

string TcpPacket::toString ()
{
	ostringstream out;
	Packet::PAYLOAD_BUFFER opts = getOptions ();

	out << "TCP packet" << std::endl
		<< "\tsource port: \t"		<< getSourceport() << std::endl
		<< "\tdest port: \t"		<< getDestport() << std::endl
		<< "\tsequencenum: \t"		<< getSequencenum() << std::endl
		<< "\tacknum: \t"		<< getAcknum() << std::endl
		<< "\theaderlen: \t"		<< getHeaderlength() << " bytes" << std::endl
		<< "\tflags: \t\t0x"		<< std::hex << std::setw(4) << std::setfill ('0') << getFlags () << std::endl << std::dec
		<< "\twindowsize: \t"		<< getWindowsize() << std::endl
		<< "\tchecksum: \t0x"		<< std::hex << std::setw(4) << std::setfill ('0') << getChecksum () << std::endl << std::dec
		<< "\turgent pnt: \t"		<< getUrgentpointer() << std::endl
		<< "\toptions len: \t"		<< opts.size << std::endl
		<< "\toptions: \t"		<< opts.toString ();

	opts.destroy ();
	return out.str ();
}

uint32_t TcpPacket::getMinProtocolSize ()
{
	return sizeof (TCP_HEADER);
}
