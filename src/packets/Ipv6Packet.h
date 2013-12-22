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

#ifndef __IPV6_PACKET_H
#define __IPV6_PACKET_H

#include "Packet.h"
#include "IpPacket.h" // needed for the transport protocol constants

#define IPV6_ADDR_LEN	16

///
/// IPv6 IP address
///

typedef struct _IPV6_ADDR {
	uint16_t x [IPV6_ADDR_LEN/2];

	//
	// print an IPv6 address according to RFC 2373, 2.2
	// http://www.ietf.org/rfc/rfc2373.txt
	//

	string toString () {
		ostringstream out;

		for (int i=0; i<IPV6_ADDR_LEN/2; i++) {

			out << std::hex << swap16 (x[i]);
			if ((i < IPV6_ADDR_LEN/2 - 1)) out << ":";

		} // for (int i=0; i<IPV6_ADDR_LEN/2; i++) 

		return out.str ();
	}
} IPV6_ADDR, *PIPV6_ADDR;

//
// IPv6 header
//

typedef struct _IPV6_HEADER {
	uint32_t	version_trafficclass_flowlabel;	// configuration item Ipv6Trafficclass and Ipv6Flowlabel
	uint16_t	payloadlen;	
	uint8_t	nextheader;
	uint8_t	hoplimit;	// configuration item Ipv6Hoplimit
	IPV6_ADDR	sourceaddr;	// configuration item Ipv6Sourceaddr
	IPV6_ADDR	destaddr;	// configuration item Ipv6Destaddr
} IPV6_HEADER, *PIPV6_HEADER;

//
// macros to fetch values from bytes
//

#define IPV6_VERSION(x) 	((x & 0xF0000000) >> 28)
#define IPV6_TRAFFICCLASS(x)	((x & 0x0FF00000) >> 20)
#define IPV6_FLOWLABEL(x)	((x & 0x000FFFFF)      )

//
// IPv6Packet class representing an IPv6 packet
//

class Ipv6Packet : public Packet {
	
public:

	Ipv6Packet();
	~Ipv6Packet();

	bool			parsePacket		();
	void			assemblePacket		();
	string			toString		();
	uint32_t		getMinProtocolSize	();

	uint8_t		getVersion		();
	uint8_t		getTrafficclass		();
	uint32_t		getFlowlabel		();
	uint16_t		getPayloadlen		();
	uint8_t		getNextheader		();
	uint8_t		getHoplimit		();
	IPV6_ADDR		getSourceaddr		();
	IPV6_ADDR		getDestaddr		();
	
	void			setVersion		(uint8_t 	version		);
	void			setTrafficclass		(uint8_t 	trafficclass	);
	void			setFlowlabel		(uint32_t 	flowlabel	);
	void			setPayloadlen		(uint16_t payloadlen	);
	void			setNextheader		(uint8_t 	nextheader	);
	void			setHoplimit		(uint8_t 	hoplimit	);
	void			setSourceaddr		(IPV6_ADDR 	sourceaddr	);
	void			setDestaddr		(IPV6_ADDR 	destaddr	);
			
	static AnonPrimitive*	anonTrafficclass;
	static AnonPrimitive*	anonFlowlabel;
	static AnonPrimitive*	anonHoplimit;
	static AnonPrimitive*	anonSourceaddr;
	static AnonPrimitive*	anonDestaddr;

private:
		
	IPV6_HEADER		header;

};

#endif // __IPV6_PACKET_H

