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

#include "Packet.h"
#include "PacketFactory.h" // this include _must_ be here because we have a circular inclusion

Packet::Packet(void)
:	buffer		(NULL),
	size		(0),
	layersize	(0),
	protocol	(PROTO_NONE),
	nextProtocol	(PROTO_NONE),
	nextPacket	(NULL),
	checksumgood	(true)
{
}

Packet::~Packet(void)
{
	if (nextPacket != NULL)
		PacketFactory::instance()->freePacket (nextPacket);

	nextPacket = NULL;

	if (buffer != NULL)
		free  (buffer);

	buffer = NULL;
}

void Packet::setSize(uint32_t len)
{
	if (len <= 0) {

		buffer = NULL;

	} else {

		if (size == 0)
			buffer = (uint8_t*) malloc (len);
		else
			buffer = (uint8_t*) realloc (buffer, len);

	}

	size = len;
}

uint8_t* Packet::getBuffer()
{
	return buffer;
}

uint32_t Packet::getSize()
{
	return size;
}

bool Packet::createNextLayer()
{
	bool ret = true;

	//
	// create the next packet object
	//

	nextPacket = PacketFactory::instance()->createPacket (nextProtocol);
	if (nextPacket == NULL) return false;

	//
	// some protocols need special handling because they 
	// need information from layer n-1
	//

	switch (nextProtocol) {
		case Packet::PROTO_TCP:
			{
				if (this->protocol == Packet::PROTO_IP)
					((TcpPacket*)nextPacket)->setIpAddresses (((IpPacket*)this)->getSourceip(), ((IpPacket*)this)->getDestip());
				else if (this->protocol == Packet::PROTO_IPV6)
					((TcpPacket*)nextPacket)->setIpAddresses (((Ipv6Packet*)this)->getSourceaddr(), ((Ipv6Packet*)this)->getDestaddr());

				break;
			}
		case Packet::PROTO_UDP:
			{	
				if (this->protocol == Packet::PROTO_IP)
					((UdpPacket*)nextPacket)->setIpAddresses (((IpPacket*)this)->getSourceip(), ((IpPacket*)this)->getDestip());
				else if (this->protocol == Packet::PROTO_IPV6)
					((UdpPacket*)nextPacket)->setIpAddresses (((Ipv6Packet*)this)->getSourceaddr(), ((Ipv6Packet*)this)->getDestaddr());

				break;
			}
		default: 
			{
				// this default is needed to shut up g++
				break;
			}
	} // switch (nextProtocol) 

	//
	// if we have space in the buffer then there may be
	// more packets, create them, else, abort
	//

	if (size <= layersize ) {
		nextProtocol = Packet::PROTO_NONE;	
		
		if (nextPacket != NULL)
			PacketFactory::instance()->freePacket (nextPacket); 

		nextPacket = NULL;
		return true;
	}

	//
	// not enought data left to create the new layer
	// maybe a incomplete packet
	//

	if (nextPacket->getMinProtocolSize() > (size - layersize)) {
		nextProtocol = Packet::PROTO_NONE;	
		
		if (nextPacket != NULL)
			PacketFactory::instance()->freePacket (nextPacket); 

		nextPacket = NULL;
		return true;
	}

	//
	// create a new packet
	//

	nextPacket->setSize (size - layersize);
	memcpy(nextPacket->getBuffer(), getBuffer() + layersize, nextPacket->size);

	nextPacket->protocol = nextProtocol;

	ret |= nextPacket->parsePacket		();
	ret |= nextPacket->createNextLayer	();

	return ret;
}

Packet::PROTOCOL Packet::getProtocol () const
{
	return protocol;
}

Packet*	Packet::getNextPacket() const
{
	return nextPacket;
}

void Packet::setNextPacket (Packet* nextpacket)
{
	nextPacket = nextpacket;
}	

void Packet::setNextProtocol(PROTOCOL proto)
{
	nextProtocol = proto;
}

uint16_t Packet::checksum(uint8_t* databuf, int len)
{
	const uint16_t* buf = (const uint16_t *)databuf;
	int length = len;
	uint32_t result = 0;

	while (length > 1) {
		result += *(buf++);
		length -= sizeof(*buf);
	}

	if (length) 
		result += *(uint8_t*)buf;

	result  = (result >> 16) + (result & 0xFFFF);
	result += (result >> 16);
	result  = (~result)&0xFFFF;

	return (uint16_t)result;
}

bool Packet::getChecksumgood () const
{
	return checksumgood;
}

void Packet::setChecksumgood (bool good)
{	
	checksumgood = good;
}

string Packet::getProtocolName (PROTOCOL proto)
{	
	string ret = "unknown";

	switch (proto) {
		case PROTO_NONE:			ret = "None";		break;
		case PROTO_DATA_PAYLOAD:	ret = "Payload";	break;
		case PROTO_ETHERNET:		ret = "Ethernet";	break;
		case PROTO_ARP:				ret = "ARP";		break;
		case PROTO_IP:				ret = "IPv4";		break;
		case PROTO_IPV6:			ret = "IPv6";		break;
		case PROTO_TCP:				ret = "TCP";		break;
		case PROTO_UDP:				ret = "UDP";		break;
		case PROTO_ICMP:			ret = "ICMP";		break;
		case PROTO_VLAN:			ret = "VLAN";		break;
	}

	return ret;
}
