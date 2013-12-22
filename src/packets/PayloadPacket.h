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

#ifndef __PAYLOAD_PACKET_H
#define __PAYLOAD_PACKET_H

#include "Packet.h"
#include <cassert>

class PayloadPacket : public Packet {
public:
	PayloadPacket();
	~PayloadPacket(void);

	bool				parsePacket		();
	void				assemblePacket		();
	string				toString		();
	uint32_t			getMinProtocolSize	();

	Packet::PAYLOAD_BUFFER		getData			();
	void				setData			(uint8_t* buf, int len);

	static AnonPrimitive*		anonPayload;
	uint32_t			getPseudoLength		();

private:

	friend class			AnonPrimitiveShorten;
	uint32_t			pseudolength;

	Packet::PAYLOAD_BUFFER		data;	// configuration item PayloadPacketData

};

#endif // __PAYLOAD_PACKET_H
