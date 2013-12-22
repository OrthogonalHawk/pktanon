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

#include "UdpPacket.h"

AnonPrimitive*	UdpPacket::anonSourceport	= NULL;
AnonPrimitive*	UdpPacket::anonDestport		= NULL;

UdpPacket::UdpPacket(void)
{
	memset		(&header, 0, sizeof (UDP_HEADER));
	protocol	= Packet::PROTO_UDP;
}

UdpPacket::~UdpPacket(void)
{
}

bool UdpPacket::parsePacket()
{
	memcpy (&header, buffer, sizeof (UDP_HEADER)); 

	//
	// what is the next protocol and the size of this protocol?
	//

	nextProtocol	= Packet::PROTO_DATA_PAYLOAD;
	layersize	= sizeof (UDP_HEADER);

	//
	// check for correct checksum
	//

	if (Configuration::instance()->getSetBadChecksumsToBad ()) {

		uint16_t chksum	= header.checksum;
		header.checksum		= 0;

		void* pipsource	= (ipProtocol == Packet::PROTO_IP ? (void*)&sourceip : (void*)&sourceip6);
		void* pipdest	= (ipProtocol == Packet::PROTO_IP ? (void*)&destip : (void*)&destip6);
	
		this->checksumgood = (checksum ( &header, pipsource, pipdest, ipProtocol, getBuffer () + layersize, getSize() - layersize) == chksum);
		header.checksum	= chksum;

	} // if (Configuration::instance()->getSetBadChecksumsToBad ()) 

	//
	// adjust endianess
	//

	header.destport		= swap16(header.destport);
	header.sourceport	= swap16(header.sourceport);
	header.checksum		= swap16(header.checksum);
	header.len		= swap16(header.len);
	
	return true;
}

uint16_t UdpPacket::getSourceport()
{
	return header.sourceport;
}

uint16_t UdpPacket::getDestport()
{
	return header.destport;
}
		
uint16_t UdpPacket::getLen()
{
	return header.len;
}
						
uint16_t UdpPacket::getChecksum()
{
	return header.checksum;
}
		
void UdpPacket::setSourceport(uint16_t sp)
{
	header.sourceport = sp;
}

void UdpPacket::setDestport(uint16_t dp)
{
	header.destport = dp;
}
		
void UdpPacket::setLen(uint16_t len)
{
	header.len = len;
}	
						
void UdpPacket::setChecksum(uint16_t chksum)
{
	header.checksum = chksum;
}

void UdpPacket::assemblePacket()
{
	if (nextPacket != NULL)
		nextPacket->assemblePacket ();

	int thissize = sizeof (UDP_HEADER) ;
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	//
	// correct header length
	//

	setLen (thissize + nextsize);

	//
	// correct endianess for output buffer
	//
	
	header.destport		= swap16(header.destport);
	header.sourceport	= swap16(header.sourceport);
	header.checksum		= swap16(header.checksum);
	header.len		= swap16(header.len);
	
	//
	// calculate new checksum. therefore we need the PayloadPacket
	// we only calculate if the old checksum was not turned off (0x0000)
	//

	if (header.checksum != 0x0000 && Configuration::instance()->getReCalcChecksums ()) {

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

			setChecksum(0);
			setChecksum(checksum (&header, pipsource, pipdest, ipProtocol, data, datalen));

		} // if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood) 

	} // if (header.checksum != 0x0000 && Configuration::instance()->getReCalcChecksums ()) 

	//
	// copy into buffer
	//

	memcpy (buffer, &header, sizeof (UDP_HEADER));

	//
	// convert endianess back
	//

	header.destport		= swap16(header.destport);
	header.sourceport	= swap16(header.sourceport);
	header.checksum		= swap16(header.checksum);
	header.len		= swap16(header.len);
}

uint16_t UdpPacket::checksum (PUDP_HEADER header, void* sourceip, void* destip, Packet::PROTOCOL ipproto, uint8_t* data, int datalen)
{
	//
	// create a pseudoheader
	// http://www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm
	//
	
	if (datalen < 0) return 0;

	int udplen = sizeof (UDP_HEADER) + datalen;

	PUDP_PSEUDO_HEADER pseudoheader	= NULL;
	uint32_t pseudolength	= 0;

	if (ipproto == Packet::PROTO_IP)	{
		pseudoheader = new UDP_IP4_PSEUDO_HEADER ();
		pseudolength = sizeof (UDP_IP4_PSEUDO_HEADER);
		
		memcpy (&((PUDP_IP4_PSEUDO_HEADER)pseudoheader)->sourceip, sourceip, IP_ADDR_LEN);
		memcpy (&((PUDP_IP4_PSEUDO_HEADER)pseudoheader)->destip,   destip,   IP_ADDR_LEN);

	} else {
		pseudoheader = new UDP_IP6_PSEUDO_HEADER ();
		pseudolength = sizeof (UDP_IP6_PSEUDO_HEADER);

		memcpy (&((PUDP_IP6_PSEUDO_HEADER)pseudoheader)->sourceip, sourceip, IPV6_ADDR_LEN);
		memcpy (&((PUDP_IP6_PSEUDO_HEADER)pseudoheader)->destip,   destip,   IPV6_ADDR_LEN);
	}
	
	pseudoheader->reserved	= 0;
	pseudoheader->protocol	= IPTYPE_UDP;
	pseudoheader->udplength	= swap16 (udplen);

	//
	// create a data buffer
	//

	int		buflen	= pseudolength + udplen;
	uint8_t*	buf	= (uint8_t*) malloc (buflen);

	memset (buf, 0, buflen);

	//
	// copy pseudoheader header, data into buffer
	// 

	memcpy (buf, pseudoheader,  pseudolength);
	memcpy (buf + pseudolength, header, sizeof (UDP_HEADER));
	memcpy (buf + pseudolength + sizeof (UDP_HEADER), data, datalen);

	//
	// create the checksum
	//

	uint16_t sum = Packet::checksum (buf, buflen);

	free (buf);
	delete pseudoheader;

	//
	// if the checksum calculates to 0x0000 we have to set it to 0xFFFF
	//

	if (sum == 0x0000)
		sum = 0xFFFF;

	return sum;
}

void UdpPacket::setIpAddresses (IP_ADDR source, IP_ADDR dest)
{
	sourceip	= source;
	destip		= dest;
	ipProtocol	= Packet::PROTO_IP;
}

void UdpPacket::setIpAddresses (IPV6_ADDR source, IPV6_ADDR dest)
{
	sourceip6	= source;
	destip6		= dest;
	ipProtocol	= Packet::PROTO_IPV6;
}

string UdpPacket::toString ()
{
	ostringstream out;

	out << "UDP packet"		<< std::endl
		<< "\tsource port: \t"	<< getSourceport ()	<< std::endl
		<< "\tdest port: \t"	<< getDestport ()	<< std::endl
		<< "\tlength: \t"	<< getLen ()		<< std::endl
		<< "\tchecksum: \t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << getChecksum() << std::dec << std::endl;

	return out.str ();
}

uint32_t UdpPacket::getMinProtocolSize ()
{
	return sizeof (UDP_HEADER);
}
