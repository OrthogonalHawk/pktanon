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

#ifndef __IP_PACKET_H
#define __IP_PACKET_H

#include "Packet.h"

//
// constants for the encapsulated transport protocols
//

#define IPTYPE_IP	0x04
#define IPTYPE_TCP	0x06
#define IPTYPE_UDP	0x11
#define IPTYPE_ICMP	0x01
#define IPTYPE_IPV6	0x29

//
// some ip constants
//

#define	IP_ADDR_LEN			4
#define IP_HEADER_NO_OPTIONS_LEN	20

//
// masks to fetch combined values
// 

#define IP_HEADER_VERSION(x)		((x &	0xF0	) >>  4	)
#define IP_HEADER_LEN(x)		((x &	0x0F	) *   4	)
#define IP_HEADER_FLAGS(x)		((x &	0xE000	) >> 13	)
#define IP_HEADER_FRAGOFFSET(x)		((x &	0x1FFF	)	)

//
// the IP header
// 

#pragma pack (1)
typedef struct _IP_HEADER {
	uint8_t		version_len;		// 4 bit version and 4 bit length 
	uint8_t		typeofservice;		// type of service . configuration item IpTos
	uint16_t		totallen;		// total length
	uint16_t		identification;		// identification. configuration item IpIdent
	uint16_t		flags_fragoffset;	// 3  bit flags and 13 bit fragment offset field. configuration item IpFragoffset and IpFlags
	uint8_t		ttl;			// time to live. configuration item IpTtl				
	uint8_t		protocol;		// protocol. configuration item IpSourceip
	uint16_t		checksum;		// checksum
	uint32_t		sourceip;		// source ip address. configuration item IpSource
	uint32_t		destip;			// dest ip address. configuration item IpDestip
} IP_HEADER, *PIP_HEADER;
#pragma pack ()

//
// a IP address
//

typedef struct _IP_ADDR {
	uint8_t x [IP_ADDR_LEN];

	string toString () {
		ostringstream out;

		for (int i=0; i<IP_ADDR_LEN; i++) {
			out << (int)x[i];
			if (i < IP_ADDR_LEN-1) out << ".";
		}

		return out.str ();
	}
} IP_ADDR, *PIP_ADDR;

//
// IP packet class
//

class IpPacket : public Packet
{
public:
	IpPacket(void);
	~IpPacket(void);

	bool			parsePacket		();
	void			assemblePacket		();
	string			toString		();
	uint32_t		getMinProtocolSize	();

	static Packet::PROTOCOL	getTransportProtocol	(uint8_t proto);

	uint8_t		getVersion		();
	uint8_t		getHeaderlength		();
	uint8_t		getTos			();
	uint16_t		getTotallen		();
	uint16_t		getIdentification	();
	uint16_t		getFlags		();
	uint16_t		getFragoffset		();
	uint8_t		getTtl			();
	uint8_t		getProtocol		();
	uint16_t		getChecksum		();
	IP_ADDR			getSourceip		();
	IP_ADDR			getDestip		();
	Packet::PAYLOAD_BUFFER	getOptions		();

	void			setVersion		(uint8_t	ver);
	void			setHeaderlength		(uint8_t	len);
	void			setTos			(uint8_t	tos);
	void			setTotallen		(uint16_t	totlen);
	void			setIdentification	(uint16_t	ident);
	void			setFlags		(uint16_t	flgs);
	void			setFragoffset		(uint16_t	fragoff);
	void			setTtl			(uint8_t	tl);
	void			setProtocol		(uint8_t	proto);
	void			setChecksum		(uint16_t checks);
	void			setSourceip		(IP_ADDR sip);
	void			setDestip		(IP_ADDR dip);
	void			setOptions		(uint8_t* buf, int len);

	static AnonPrimitive*	anonTos;
	static AnonPrimitive*	anonIdentifier;
	static AnonPrimitive*	anonFlags;
	static AnonPrimitive*	anonFragoffset;
	static AnonPrimitive*	anonTtl;
	static AnonPrimitive*	anonSourceip;
	static AnonPrimitive*	anonDestip;
	static AnonPrimitive*	anonOptions;

private:

	IP_HEADER		header;
	Packet::PAYLOAD_BUFFER	options; // configuration item IpOptions
	
protected:

	static uint16_t	checksum		(PIP_HEADER ipheader);

};

#endif // __IP_PACKET_H
