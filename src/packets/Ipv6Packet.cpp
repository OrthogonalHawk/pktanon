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

#include "Ipv6Packet.h"
#include "TcpPacket.h"
#include "UdpPacket.h"

AnonPrimitive* Ipv6Packet::anonTrafficclass	= NULL;
AnonPrimitive* Ipv6Packet::anonFlowlabel	= NULL;
AnonPrimitive* Ipv6Packet::anonHoplimit		= NULL;
AnonPrimitive* Ipv6Packet::anonSourceaddr	= NULL;
AnonPrimitive* Ipv6Packet::anonDestaddr		= NULL;

Ipv6Packet::Ipv6Packet ()
{
	memset	 (&header, 0, sizeof (IPV6_HEADER));
	protocol = Packet::PROTO_IPV6;
}

Ipv6Packet::~Ipv6Packet ()
{	
}

bool Ipv6Packet::parsePacket ()
{
	memcpy		(&header, buffer, sizeof (IPV6_HEADER)); 
	layersize	= sizeof (IPV6_HEADER);

	header.version_trafficclass_flowlabel	= swap32 (header.version_trafficclass_flowlabel);
	header.payloadlen			= swap16 (header.payloadlen);

	nextProtocol = IpPacket::getTransportProtocol (header.nextheader);

	//
	// remove ethernet padding
	//

	if (getSize () > sizeof (IPV6_HEADER) + header.payloadlen)
		this->size = sizeof (IPV6_HEADER) + header.payloadlen;

	return true;
}

void Ipv6Packet::assemblePacket ()
{
	if (nextPacket != NULL) {
		
		// TCP and UDP packets need the source
		// and dest IP addresses to calc their 
		// checksums. So we pass it over here
		// where it is already transformed.

		if (nextPacket->getProtocol() == Packet::PROTO_TCP) {
			((TcpPacket*)nextPacket)->setIpAddresses (getSourceaddr(), getDestaddr());
		} else if (nextPacket->getProtocol() == Packet::PROTO_UDP) {
			((UdpPacket*)nextPacket)->setIpAddresses (getSourceaddr(), getDestaddr());
		}

		nextPacket->assemblePacket ();
	}

	int thissize = sizeof (IPV6_HEADER);
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	setPayloadlen (nextsize);

	header.version_trafficclass_flowlabel	= swap32 (header.version_trafficclass_flowlabel);
	header.payloadlen			= swap16 (header.payloadlen);

	memcpy (buffer, &header, sizeof (IPV6_HEADER));

	header.version_trafficclass_flowlabel	= swap32 (header.version_trafficclass_flowlabel);
	header.payloadlen			= swap16 (header.payloadlen);
}

string Ipv6Packet::toString ()
{
	ostringstream out;

	out << "IPv6 packet"			<< std::endl
		<< "\tVersion: \t"		<< (int)getVersion()		<< std::endl
		<< "\tTrafficclass: \t0x"	<< std::hex << std::setw(2)	<< std::setfill ('0') << (int) getTrafficclass() << std::endl << std::dec
		<< "\tFlowlabel: \t0x"		<< std::hex << std::setw(5)	<< std::setfill ('0') << getFlowlabel() << std::endl << std::dec
		<< "\tPayloadlen: \t"		<< getPayloadlen()		<< std::endl
		<< "\tNextheader: \t0x"		<< std::hex << std::setw(2)	<< std::setfill ('0') << (int) getNextheader()	<< std::endl << std::dec
		<< "\tHoplimit: \t"		<< (int)getHoplimit()		<< std::endl
		<< "\tSourceaddr: \t"		<< getSourceaddr().toString()	<< std::endl
		<< "\tDestaddr: \t"		<< getDestaddr().toString();

	return out.str ();
}

uint32_t Ipv6Packet::getMinProtocolSize ()
{
	return sizeof (IPV6_HEADER);
}

uint8_t Ipv6Packet::getVersion ()
{
	return IPV6_VERSION (header.version_trafficclass_flowlabel);
}

uint8_t Ipv6Packet::getTrafficclass ()
{
	return IPV6_TRAFFICCLASS (header.version_trafficclass_flowlabel);
}

uint32_t Ipv6Packet::getFlowlabel ()
{
	return IPV6_FLOWLABEL (header.version_trafficclass_flowlabel);
}

uint16_t Ipv6Packet::getPayloadlen ()
{
	return header.payloadlen;
}

uint8_t Ipv6Packet::getNextheader ()
{
	return header.nextheader;
}

uint8_t Ipv6Packet::getHoplimit ()
{
	return header.hoplimit;	
}

IPV6_ADDR Ipv6Packet::getSourceaddr ()
{
	return header.sourceaddr;
}

IPV6_ADDR Ipv6Packet::getDestaddr ()
{
	return header.destaddr;	
}

void Ipv6Packet::setVersion (uint8_t version)
{
	header.version_trafficclass_flowlabel = (((uint32_t)version & 0xF) << 28) | (header.version_trafficclass_flowlabel & 0x0FFFFFFF);
}

void Ipv6Packet::setTrafficclass (uint8_t trafficclass)
{
	header.version_trafficclass_flowlabel = ((uint32_t)trafficclass << 20) | (header.version_trafficclass_flowlabel & 0xF00FFFFF);
}

void Ipv6Packet::setFlowlabel (uint32_t flowlabel)
{
	header.version_trafficclass_flowlabel = (flowlabel & 0x000FFFFF) | (header.version_trafficclass_flowlabel & 0xFFF00000);
}

void Ipv6Packet::setPayloadlen (uint16_t payloadlen)
{
	header.payloadlen = payloadlen;
}

void Ipv6Packet::setNextheader (uint8_t nextheader)
{
	header.nextheader = nextheader;
}

void Ipv6Packet::setHoplimit (uint8_t hoplimit)
{
	header.hoplimit = hoplimit;
}

void Ipv6Packet::setSourceaddr (IPV6_ADDR sourceaddr)
{
	header.sourceaddr = sourceaddr;
}

void Ipv6Packet::setDestaddr (IPV6_ADDR destaddr)
{
	header.destaddr = destaddr;
}
