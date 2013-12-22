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

#include "ArpPacket.h"
		
AnonPrimitive*	ArpPacket::anonHardwaretype		= NULL;
AnonPrimitive*	ArpPacket::anonProtocoltype		= NULL;
AnonPrimitive*	ArpPacket::anonHardwareaddrlen		= NULL;
AnonPrimitive*	ArpPacket::anonProtoaddrlen		= NULL;
AnonPrimitive*	ArpPacket::anonOpcode			= NULL;
AnonPrimitive*	ArpPacket::anonSourcemac		= NULL;
AnonPrimitive*	ArpPacket::anonSourceip			= NULL;
AnonPrimitive*	ArpPacket::anonDestmac			= NULL;
AnonPrimitive*	ArpPacket::anonDestip			= NULL;
	
ArpPacket::ArpPacket ()
{
	memset		(&header, 0, sizeof (ARP_HEADER));
	checksumgood 	= true;
	protocol	= Packet::PROTO_ARP;
	layersize	= sizeof (ARP_HEADER);
}

ArpPacket::~ArpPacket ()
{	
}

bool ArpPacket::parsePacket ()
{
	memcpy (&header, buffer, sizeof (ARP_HEADER));

	header.hardwaretype = swap16 (header.hardwaretype);
	header.protocoltype = swap16 (header.protocoltype);
	header.opcode = swap16 (header.opcode);

	nextProtocol = Packet::PROTO_NONE;
	layersize = sizeof (ARP_HEADER);

	return true;
}

void ArpPacket::assemblePacket ()
{
	setSize (sizeof (ARP_HEADER));

	header.hardwaretype = swap16 (header.hardwaretype);
	header.protocoltype = swap16 (header.protocoltype);
	header.opcode = swap16 (header.opcode);

	memcpy (buffer, &header, sizeof (ARP_HEADER));

	header.hardwaretype = swap16 (header.hardwaretype);
	header.protocoltype = swap16 (header.protocoltype);
	header.opcode = swap16 (header.opcode);
	}

string ArpPacket::toString ()
{
	ostringstream out;

	out 	<< "ARP packet"			<< std::endl
		<< "\thardware type: \t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << header.hardwaretype << std::endl << std::dec
		<< "\tprotocol type: \t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << header.protocoltype << std::endl << std::dec
		<< "\thw addr len: \t"		<< (uint32_t)header.hwaddrlen << std::endl
		<< "\tprot addr len: \t"	<< (uint32_t)header.protoaddrlen << std::endl
		<< "\topcode: \t0x"		<< std::hex << std::setw (4) << std::setfill ('0') << header.opcode << std::endl << std::dec
		<< "\tsource mac: \t"		<< header.sourcemac.toString() << std::endl
		<< "\tsource ip: \t"		<< header.sourceip.toString() << std::endl
		<< "\tdest mac: \t"		<< header.destmac.toString() << std::endl
		<< "\tdest ip: \t"		<< header.destip.toString() << std::endl;

	return out.str ();
}

uint32_t ArpPacket::getMinProtocolSize ()
{
	return sizeof (ARP_HEADER);
}

uint16_t ArpPacket::getHardwaretype ()
{
	return header.hardwaretype;	
}

uint16_t ArpPacket::getProtocoltype ()
{
	return header.protocoltype;
}

uint8_t ArpPacket::getHardwareaddrlen ()
{
	return header.hwaddrlen;
}

uint8_t ArpPacket::getProtocoladdrlen ()
{
	return header.protoaddrlen;
}

uint16_t ArpPacket::getOpcode ()
{
	return header.opcode;
}

MAC_ADDR ArpPacket::getSourcemac ()
{
	return header.sourcemac;
}

IP_ADDR	ArpPacket::getSourceip ()
{
	return header.sourceip;
}

MAC_ADDR ArpPacket::getDestmac ()
{
	return header.destmac;
}

IP_ADDR	ArpPacket::getDestip ()
{
	return header.destip;
}

void ArpPacket::setHardwaretype (uint16_t hwtype)	
{
	header.hardwaretype = hwtype;
}

void ArpPacket::setProtocoltype (uint16_t	prototype)
{
	header.protocoltype = prototype;
}

void ArpPacket::setHardwareaddrlen (uint8_t hwlen)
{
	header.hwaddrlen = hwlen;
}

void ArpPacket::setProtocoladdrlen (uint8_t protolen)
{
	header.protoaddrlen = protolen;
}

void ArpPacket::setOpcode (uint16_t opcode)
{
	header.opcode = opcode;
}

void ArpPacket::setSourcemac (MAC_ADDR sourcemac)
{
	header.sourcemac = sourcemac;
}

void ArpPacket::setSourceip (IP_ADDR sourceip)
{
	header.sourceip = sourceip;	
}

void ArpPacket::setDestmac (MAC_ADDR destmac)
{
	header.destmac = destmac;
}

void ArpPacket::setDestip (IP_ADDR destip)
{
	header.destip = destip;
}
