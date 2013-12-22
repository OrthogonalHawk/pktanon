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

#ifndef __ICMP_PACKET_H
#define __ICMP_PACKET_H

#include "Packet.h"
#include "PayloadPacket.h"

//
// ICMP header
//

#pragma pack (1)
typedef struct _ICMP_HEADER {
	uint8_t	type;		// type of message. configuration item IcmpType
	uint8_t	code;		// type sub code. configuration item IcmpCode 
	uint16_t	checksum;	// checksum
	uint32_t	misc;		// depending on type and code. configuration item IcmpMisc
} ICMP_HEADER, *PICMP_HEADER;
#pragma pack ()

//
// ICMP packet class
//

class IcmpPacket : public Packet
{
public:
	IcmpPacket(void);
	~IcmpPacket(void);

	bool			parsePacket		();
	void			assemblePacket		();
	string			toString		();
	uint32_t		getMinProtocolSize	();

	uint8_t		getType			();
	uint8_t		getCode			();
	uint16_t		getChecksum		();
	Packet::PAYLOAD_BUFFER	getData			();
	uint32_t		getMisc			();

	void			setType			(uint8_t tp);
	void			setCode			(uint8_t cd);
	void			setChecksum		(uint16_t chk);
	void			setData			(Packet::PAYLOAD_BUFFER buf);
	void			setMisc			(uint32_t msc);

	static AnonPrimitive*	anonType;
	static AnonPrimitive*	anonCode;
	static AnonPrimitive*	anonMisc;

private:

	ICMP_HEADER		header;

	static uint16_t checksum (PICMP_HEADER header, uint8_t* data, int datalen);

};

#endif // __ICMP_PACKET_H
