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

#ifndef __UDP_PACKET_H
#define __UDP_PACKET_H

#include "Packet.h"
#include "IpPacket.h"
#include "Ipv6Packet.h"
#include "PayloadPacket.h"

//
// a UDP header
//

#pragma pack (1)
typedef struct _UDP_HEADER {
	uint16_t	sourceport;	// source port. configuration item UdpSourceport
	uint16_t	destport;	// destination port. configuration item UdpDestport
	uint16_t	len;		// udp length 
	uint16_t	checksum;	// udp checksum 
} UDP_HEADER, *PUDP_HEADER  ;
#pragma pack ()

//
// UDP pseudo header, needed for checksumming
//

#pragma pack (1)
typedef struct _UDP_PSEUDO_HEADER {
	uint8_t	reserved;
	uint8_t	protocol;
	uint16_t	udplength;
} UDP_PSEUDO_HEADER, *PUDP_PSEUDO_HEADER;

typedef struct _UDP_IP4_PSEUDO_HEADER : public UDP_PSEUDO_HEADER {
	IP_ADDR		sourceip;
	IP_ADDR		destip;
} UDP_IP4_PSEUDO_HEADER, *PUDP_IP4_PSEUDO_HEADER;

typedef struct _UDP_IP6_PSEUDO_HEADER : public UDP_PSEUDO_HEADER {
	IPV6_ADDR	sourceip;
	IPV6_ADDR	destip;
} UDP_IP6_PSEUDO_HEADER, *PUDP_IP6_PSEUDO_HEADER;
#pragma pack ()

//
// UdpPacket class representing a udp packet
//

class UdpPacket : public Packet
{
public:
	UdpPacket(void);
	~UdpPacket(void);

	bool			parsePacket		();
	void			assemblePacket		();
	string			toString		();
	uint32_t		getMinProtocolSize	();	

	uint16_t		getSourceport		();
	uint16_t		getDestport		();
	uint16_t		getLen			();
	uint16_t		getChecksum		();

	void			setSourceport		(uint16_t sp);
	void			setDestport		(uint16_t dp);
	void			setLen			(uint16_t len);
	void			setChecksum		(uint16_t chksum);

	void			setIpAddresses		(IP_ADDR source, IP_ADDR dest);
	void			setIpAddresses		(IPV6_ADDR source, IPV6_ADDR dest);

	static AnonPrimitive*	anonSourceport;	
	static AnonPrimitive*	anonDestport;

private:
	UDP_HEADER		header;	

	// the source and dest IP, we need this to calculate the checksum
	IP_ADDR			sourceip, destip;
	IPV6_ADDR		sourceip6, destip6;
	Packet::PROTOCOL	ipProtocol;

	// calculate the UDP checksum
	static uint16_t	checksum (PUDP_HEADER header, void* sourceip, void* destip, Packet::PROTOCOL ipproto, uint8_t* data, int datalen);
};

#endif // __UDP_PACKET_H
