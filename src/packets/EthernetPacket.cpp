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

#include "EthernetPacket.h"

AnonPrimitive* EthernetPacket::anonSourceMac	= NULL;
AnonPrimitive* EthernetPacket::anonDestMac	= NULL;
AnonPrimitive* EthernetPacket::anonType		= NULL;

EthernetPacket::EthernetPacket(void)
{
	protocol	= Packet::PROTO_ETHERNET;
	memset		(&header, 0, sizeof (ETHERNET_HEADER));
}

EthernetPacket::~EthernetPacket(void)
{
}

bool EthernetPacket::parsePacket()
{
	if (buffer == NULL) return false;

	if ((uint32_t)size < sizeof (ETHERNET_HEADER)) {
		cerr << "packet too small to contain ethernet header (" << size << "bytes)" << std::endl;
		return false;
	}

	memcpy (&header, buffer, sizeof (ETHERNET_HEADER));
	header.type = swap16 (header.type);

	switch (header.type) {
		case ETHERTYPE_IP:		nextProtocol = Packet::PROTO_IP; break;
		case ETHERTYPE_ARP:		nextProtocol = Packet::PROTO_ARP; break;
		case ETHERTYPE_IPV6:	nextProtocol = Packet::PROTO_IPV6; break;
		case ETHERTYPE_VLAN:	nextProtocol = Packet::PROTO_VLAN; break;
		case ETHERTYPE_UNKNOWN:
		default:				nextProtocol = Packet::PROTO_DATA_PAYLOAD; break;
	}

	layersize = sizeof (ETHERNET_HEADER);
	return true;
}

MAC_ADDR EthernetPacket::getSourceMac ()
{
	MAC_ADDR ret = {{0}};
	memcpy (&ret, &header.sourcehost, ETHER_ADDR_LEN);
	return ret;
}

MAC_ADDR EthernetPacket::getDestMac ()
{
	MAC_ADDR ret = {{0}};
	memcpy (&ret, &header.desthost, ETHER_ADDR_LEN);
	return ret;
}

uint16_t EthernetPacket::getType ()
{	
	return header.type;
}

void EthernetPacket::setSourceMac (MAC_ADDR mac)
{
	memcpy (&header.sourcehost, &mac, ETHER_ADDR_LEN);
}

void EthernetPacket::setDestMac	(MAC_ADDR mac)
{
	memcpy (&header.desthost, &mac, ETHER_ADDR_LEN);
}

void EthernetPacket::setType (uint16_t tp)
{
	header.type = tp;
}

void EthernetPacket::assemblePacket()
{
	if (nextPacket != NULL)
		nextPacket->assemblePacket ();

	int thissize = sizeof(ETHERNET_HEADER);
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	//
	// ethernet needs a minimum data of 46 bytes
	// if this is not the case we will add padding.
	// this can make the output trace different from the
	// original trace because small packets that are
	// sent from the capture machine are not already
	// padded when they are captured. The padding is mostly
	// done by the ethernet nic. But we want rfc friendly
	// packets as output so we do pad.
	// http://www.ethereal.com/lists/ethereal-users/200304/msg00104.html
	//

	int padding = (nextsize < 46 ? 46 - nextsize : 0);

	//
	// allocate the buffer
	//

	setSize (thissize + nextsize + padding);
	memset	(buffer, 0, getSize ());

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	header.type = swap16 (header.type);
	memcpy (buffer, &header, sizeof(ETHERNET_HEADER));
	header.type = swap16 (header.type);
}

string EthernetPacket::toString	()
{
	ostringstream out;
	
	out << "Ethernet packet"	<< std::endl
		<< "\tsource mac: \t"	<< getSourceMac().toString() << std::endl
		<< "\tdest mac: \t"	<< getDestMac().toString() << std::endl
		<< "\ttype: \t\t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << getType();

	return out.str ();
}

uint32_t EthernetPacket::getMinProtocolSize ()
{
	return sizeof (ETHERNET_HEADER);
}
