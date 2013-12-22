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

#ifndef __FRAME_H
#define __FRAME_H

#include "EthernetPacket.h"
#include <sstream>

using std::ostringstream;

class Frame 
{
public:
	Frame	(void);
	~Frame	(void);

	typedef enum _LINK_TYPE {
		LINK_UNKNOWN	= 0,
		LINK_ETHERNET	= 1,
	} LINK_TYPE;

	#pragma pack (1)
	typedef struct _TIME_VAL {
		  int32_t tv_sec;
		  int32_t tv_usec;

		string toString () {
			ostringstream out;
			out << tv_sec << "sec " << tv_usec << "microsec";
			return out.str ();
		}
	} TIME_VAL, *PTIME_VAL;
	#pragma pack ()

	static LINK_TYPE		nettype;

	TIME_VAL			timestamp;
	uint32_t			capturelength;
	uint32_t			packetlength;

	Packet*				payloadpacket;

	Packet::PAYLOAD_BUFFER		getPayloadBuffer();
	string				toString();

};

#endif // __FRAME_H
#pragma pack (1)
