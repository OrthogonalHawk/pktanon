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

#ifndef __ARP_PACKET_H
#define __ARP_PACKET_H

#include "Packet.h"
#include "IpPacket.h"
#include "EthernetPacket.h"

//
// ARP header
//

#pragma pack (1)
typedef struct _ARP_HEADER {
	uint16_t	hardwaretype;	// 0x0001 for ethernet. configuration item ArpHardwaretp
	uint16_t	protocoltype;	// 0x0800 for ip. configuration item ArpPrototp
	uint8_t	hwaddrlen;	// 0x6 for ethernet mac addressess. configuration item ArpHardwareaddrlen
	uint8_t 	protoaddrlen;	// 0x4 for ip addresses. configuration item ArpProtoaddrlen
	uint16_t	opcode;		// see defines below. configuration item ArpOpcode
	MAC_ADDR	sourcemac;	// configuration item ArpSourcemac
	IP_ADDR		sourceip;	// configuration item ArpSourceip
	MAC_ADDR	destmac;	// configuration item ArpDestmac
	IP_ADDR		destip;		// configuration item ArpDestip
} ARP_HEADER, *PARP_HEADER;
#pragma pack ()

//
// ARP opcodes (request, reply, reverse ARP request, reverse ARP reply)
//

#define ARP_OPCODE_REQUEST		0x0001
#define ARP_OPCODE_REPLY		0x0002
#define ARP_OPCODE_REVREQUEST		0x0003
#define ARP_OPCODE_REVREPLY		0x0004

//
// ArpPacket class representing an arp packet
//

class ArpPacket : public Packet {
public:
				ArpPacket			();
				~ArpPacket			();
		
	bool			parsePacket			();
	void			assemblePacket			();
	string			toString			();
	uint32_t		getMinProtocolSize		();
		
	uint16_t		getHardwaretype 		();	
	uint16_t		getProtocoltype			();
	uint8_t		getHardwareaddrlen		();
	uint8_t		getProtocoladdrlen		();
	uint16_t		getOpcode			();
	MAC_ADDR		getSourcemac			();
	IP_ADDR			getSourceip			();
	MAC_ADDR		getDestmac			();
	IP_ADDR			getDestip			();
	
	void			setHardwaretype 		(uint16_t hwtype		);	
	void			setProtocoltype			(uint16_t	prototype	);
	void			setHardwareaddrlen		(uint8_t 	hwlen		);
	void			setProtocoladdrlen		(uint8_t 	protolen	);
	void			setOpcode			(uint16_t	opcode		);
	void			setSourcemac			(MAC_ADDR sourcemac);
	void			setSourceip			(IP_ADDR sourceip);
	void			setDestmac			(MAC_ADDR destmac);
	void			setDestip			(IP_ADDR destip);
		
	static AnonPrimitive*	anonHardwaretype;
	static AnonPrimitive*	anonProtocoltype;	
	static AnonPrimitive*	anonHardwareaddrlen;
	static AnonPrimitive*	anonProtoaddrlen;
	static AnonPrimitive*	anonOpcode;
	static AnonPrimitive*	anonSourcemac;
	static AnonPrimitive*	anonSourceip;
	static AnonPrimitive*	anonDestmac;
	static AnonPrimitive*	anonDestip;
	
private:
		
	ARP_HEADER		header;

};

#endif // __ARP_PACKET_H
