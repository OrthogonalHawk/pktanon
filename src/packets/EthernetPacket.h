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

#ifndef	__ETHERNET_HEADER_H
#define __ETHERNET_HEADER_H

#include "Packet.h"

//
// some ethernet constants
//

#define	ETHER_ADDR_LEN	6	// length of an Ethernet address

//
// Structure of a 10Mb/s Ethernet header.
//

#pragma pack (1)
typedef struct	_ETHERNET_HEADER {
	uint8_t	desthost	[ETHER_ADDR_LEN];	// configuration item MacDest
	uint8_t	sourcehost	[ETHER_ADDR_LEN];	// configuration item MacSource
	uint16_t	type;					// configuration item MacType
} ETHERNET_HEADER, *PETHERNET_HEADER ;
#pragma pack ()

//
// some ethertypes for the encapsulated network protocol
//

#define ETHERTYPE_UNKNOWN	0x0000	// unknown protocol type
#define	ETHERTYPE_IP		0x0800	// IP protocol version 4
#define	ETHERTYPE_ARP		0x0806	// Address resolution protocol 
#define	ETHERTYPE_IPV6		0x86DD	// IP protocol version 6 
#define ETHERTYPE_VLAN		0x8100	// 802.1Q Virtual Lan

//
// a MAC address
//

typedef struct _MAC_ADDR {
	uint8_t x[ETHER_ADDR_LEN];

	string toString () {
		ostringstream out;

		for (int i=0; i<ETHER_ADDR_LEN; i++) {
			out << std::hex << std::setw (2) << std::setfill ('0') << (int)x[i];
			if (i < ETHER_ADDR_LEN-1) out << "-";
		}

		return out.str ();
	}
} MAC_ADDR, *PMAC_ADDR;

//
// EthernetPacket class, representing an ethernet packet
//

class EthernetPacket : public Packet
{
public:

	EthernetPacket			(void);
	~EthernetPacket			(void);

	bool				parsePacket		();
	void				assemblePacket		();
	string				toString		();
	uint32_t			getMinProtocolSize	();

	MAC_ADDR			getSourceMac		();
	MAC_ADDR			getDestMac		();
	uint16_t			getType			();

	void				setSourceMac		(MAC_ADDR mac);
	void				setDestMac		(MAC_ADDR mac);
	void				setType			(uint16_t tp);
	
	static AnonPrimitive*		anonSourceMac;
	static AnonPrimitive*		anonDestMac;
	static AnonPrimitive*		anonType;

private:

	ETHERNET_HEADER			header;

};

#endif // __ETHERNET_HEADER_H
