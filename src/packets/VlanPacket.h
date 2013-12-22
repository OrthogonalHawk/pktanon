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

#ifndef __VLAN_PACKET_H
#define __VLAN_PACKET_H

#include "Packet.h"

//
// the VLAN tag
// 

#pragma pack (1)
typedef struct _VLAN_HEADER {
	uint16_t		priority_cfi_id;	// priority, canonical format identifier, vlan identifier
	uint16_t		type; 				// next header, based on ethernet protocol fields
} VLAN_HEADER, *PVLAN_HEADER;
#pragma pack ()

//
// Vlan packet class
//

class VlanPacket : public Packet
{
public:
	VlanPacket(void);
	~VlanPacket(void);

	bool			parsePacket();
	void			assemblePacket();
	string			toString();
	uint32_t		getMinProtocolSize();

	uint8_t 		getPriority();
	bool			getCanonicalFormatIdentifier();
	uint16_t		getVlanIdentifier();
	uint16_t		getType();

	void 			setPriority(uint8_t priority);
	void 			setCanonicalFormatIdentifier(bool cfi);
	void 			setVlanIdentifier(uint16_t	vlanid);
	void			setType(uint16_t tp);

	static AnonPrimitive*	anonPriority;
	static AnonPrimitive*	anonCfi;
	static AnonPrimitive*	anonVlanId;
	static AnonPrimitive*	anonType;

private:
	VLAN_HEADER		header;
};

#endif // __VLAN_PACKET_H
