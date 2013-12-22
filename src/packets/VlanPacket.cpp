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

#include "VlanPacket.h"
#include "EthernetPacket.h"

static AnonPrimitive*	anonPriority;
static AnonPrimitive*	anonCfi;
static AnonPrimitive*	anonVlanId;
static AnonPrimitive*	anonType;

AnonPrimitive*	VlanPacket::anonPriority	= NULL;
AnonPrimitive*	VlanPacket::anonCfi			= NULL;
AnonPrimitive*	VlanPacket::anonVlanId		= NULL;
AnonPrimitive*	VlanPacket::anonType		= NULL;

VlanPacket::VlanPacket(void)
{
	memset		(&header, 0, sizeof (VLAN_HEADER));
	protocol	= Packet::PROTO_VLAN;
}

VlanPacket::~VlanPacket(void)
{
}

bool VlanPacket::parsePacket()
{
	memcpy (&header, buffer, sizeof (VLAN_HEADER));
	header.type = swap16 (header.type);
	header.priority_cfi_id = swap16 (header.priority_cfi_id);

	switch (header.type) {
		case ETHERTYPE_IP:		nextProtocol = Packet::PROTO_IP; break;
		case ETHERTYPE_ARP:		nextProtocol = Packet::PROTO_ARP; break;
		case ETHERTYPE_IPV6:	nextProtocol = Packet::PROTO_IPV6; break;
		case ETHERTYPE_VLAN:	nextProtocol = Packet::PROTO_VLAN; break;
		case ETHERTYPE_UNKNOWN:
		default:				nextProtocol = Packet::PROTO_DATA_PAYLOAD; break;
	}

	layersize = sizeof (VLAN_HEADER);
	return true;
}

void VlanPacket::assemblePacket()
{
	if (nextPacket != NULL)
		nextPacket->assemblePacket ();

	int thissize = sizeof(VLAN_HEADER);
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	header.type = swap16 (header.type);
	header.priority_cfi_id = swap16 (header.priority_cfi_id);
	memcpy (buffer, &header, sizeof(VLAN_HEADER));
	header.type = swap16 (header.type);
	header.priority_cfi_id = swap16 (header.priority_cfi_id);
}

uint8_t VlanPacket::getPriority()
{
	return ((header.priority_cfi_id & 0xE000) >> 13);
}

bool VlanPacket::getCanonicalFormatIdentifier()
{
	return ((header.priority_cfi_id & 0x1000) != 0);
}

uint16_t VlanPacket::getVlanIdentifier()
{
	return (header.priority_cfi_id & 0x0FFF);
}

uint16_t VlanPacket::getType()
{
	return header.type;
}

void VlanPacket::setPriority(uint8_t priority)
{
	uint16_t priority16 = priority;
	header.priority_cfi_id = (priority16 & 0xE000) | (header.priority_cfi_id & 0x1FFF);
}

void VlanPacket::setCanonicalFormatIdentifier(bool cfi)
{
	uint16_t cfi16 = (cfi ? 0x0001 : 0x0000);
	header.priority_cfi_id = (cfi16 & 0x1000) | (header.priority_cfi_id & 0xEFFF);
}

void VlanPacket::setVlanIdentifier(uint16_t	vlanid)
{
	header.priority_cfi_id = (vlanid & 0x0FFF) | (header.priority_cfi_id & 0xF000);
}

void VlanPacket::setType(uint16_t tp)
{
	header.type = tp;
}

string VlanPacket::toString ()
{
	ostringstream out;

	out << "VLAN tag"		<< std::endl
		<< "\tpriority: \t"	<< (int)getPriority()		<< std::endl
		<< "\tcfi: \t\t"	<< getCanonicalFormatIdentifier() << std::endl
		<< "\tvlanid: \t"	<< getVlanIdentifier() << std::endl
		<< "\ttype: \t\t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << getType();
	
	return out.str ();
}

uint32_t VlanPacket::getMinProtocolSize ()
{
	return sizeof (VLAN_HEADER);
}
