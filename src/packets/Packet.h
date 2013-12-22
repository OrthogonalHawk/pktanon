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

#ifndef __PACKET_H
#define __PACKET_H

#include "anonprimitives/AnonPrimitive.h"
#include "Configuration.h"
#include "RandomNumberGenerator.h"

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using std::ostringstream;
using std::cerr;
using std::string;

#define swap16(x)	( ((x & 0xff) << 8) | ((x & 0xff00) >> 8)								)
#define swap32(x)	( (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24)	)

class Packet 
{
public:
	Packet			(void);
	virtual ~Packet		(void);

	bool			createNextLayer		();
	void			setSize			(uint32_t len);
	uint32_t		getSize			();
	uint8_t*		getBuffer		();

	virtual bool		parsePacket		() = 0;
	virtual void		assemblePacket		() = 0;
	virtual string		toString		() = 0;
	virtual uint32_t	getMinProtocolSize	() = 0;

	typedef enum _PROTOCOL {
		PROTO_NONE,			// no next protocol
		PROTO_DATA_PAYLOAD,	// data payload
		PROTO_ETHERNET,		// ethernet packet
		PROTO_ARP,			// arp packet
		PROTO_IP,			// ip packet
		PROTO_IPV6,			// ipv6 packet
		PROTO_TCP,			// tcp packet
		PROTO_UDP,			// udp packet
		PROTO_ICMP,			// icmp packet
		PROTO_VLAN,			// 802.1Q vlan tag
	} PROTOCOL;

	typedef struct _PAYLOAD_BUFFER {
		uint8_t*	buf;		// pointer to the data buffer
		uint32_t	size;		// the size of the buffer
		uint32_t	pseudosize;	// tell any lower protocol that uses this buffer to set its length in a length field to this pseudo size

		_PAYLOAD_BUFFER () {
			buf		= NULL;
			size		= 0;
			pseudosize	= 0;
		}

		_PAYLOAD_BUFFER (uint8_t* _buf, int _size) {
			buf		= _buf;
			size		= _size;
			pseudosize	= _size;
		}

		void destroy () {
			if (buf != NULL)
				free (buf);
		}

		string toString (bool decodestring = false) {
			if (buf == NULL || size == 0) return "";

			ostringstream out;
			uint8_t* pos = buf;
			
			if (! decodestring) {
				for (uint32_t i=0; i<size; i++, pos++) {
					out << std::hex << std::setw (2) << std::setfill ('0') << (int)*pos;
					if (i+1 != size) out << " ";
				}
			} else {
				for (uint32_t i=0; i<size; i++, pos++) 
					out << (char) *pos;
			}

			return out.str ();
		}
	} PAYLOAD_BUFFER, *PPAYLOAD_BUFFER; 

	PROTOCOL		getProtocol		() const;
	static string		getProtocolName		(PROTOCOL proto);
	Packet*			getNextPacket		() const;
	void			setNextPacket		(Packet* nextpacket);
	void			setNextProtocol		(PROTOCOL proto);

	bool			getChecksumgood		() const;
	void			setChecksumgood		(bool good);

protected:

	uint8_t*		buffer;			// buffer of the raw data of this protocol including all payload protocols
	uint32_t		size;			// size of the raw data of this protocol including all payload protocols
	uint32_t		layersize;		// size of this protocol
	PROTOCOL		protocol;		// this protocol
	PROTOCOL		nextProtocol;		// the payload protocol
	Packet*			nextPacket;		// the next payload packet

	bool			checksumgood;		// is the checksum good? Only needed for Settings/Checksumming/SetBadChecksumsToBad
	static uint16_t	checksum		(uint8_t* databuf, int len);

};

#endif // __PACKET_H
