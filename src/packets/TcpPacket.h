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

#ifndef __TCP_PACKET_H
#define __TCP_PACKET_H

#include "Packet.h"
#include "IpPacket.h"
#include "Ipv6Packet.h"
#include "PayloadPacket.h"

//
// masks to fetch combined values
// 

#define TCP_HEADER_LEN(x)	(((x & 0xF000) >> 12) * 4)
#define TCP_HEADER_FLAGS(x)	((x & 0x3F ))

//
// TCP header len without options
//

#define TCP_HEADER_NO_OPTIONS_LEN	20

//
// TCP header 
//

#pragma pack (1)
typedef struct _TCP_HEADER {
	uint16_t	sourceport;	// source port. configuration item TcpSourceport
	uint16_t	destport;	// destination port. configuration item TcpDestport 
	uint32_t	sequencenum;	// sequence number. configuration item TcpSeqnum 
	uint32_t	acknum;		// acknowledgement number. configuration item TcpAcknum 
	uint16_t	headerlen_flags;//header length 4 bit, 6 bit reserved, 6 bit flags. configuration item TcpFlags. none for header length
	uint16_t	windowsize;	// window size. configuration item TcpWindowsize
	uint16_t	checksum;	// checksum 
	uint16_t	urgentpointer;	// urgent pointer. configuration item TcpUrgentpnt 
} TCP_HEADER, *PTCP_HEADER;
#pragma pack ()

//
// TCP pseudo header, needed for checksumming
//

#pragma pack (1)
typedef struct _TCP_PSEUDO_HEADER {
	uint8_t	reserved;
	uint8_t	protocol;
	uint16_t	tcplength;
} TCP_PSEUDO_HEADER, *PTCP_PSEUDO_HEADER;

typedef struct _TCP_IP4_PSEUDO_HEADER : public TCP_PSEUDO_HEADER{
	IP_ADDR			sourceip;
	IP_ADDR			destip;
} TCP_IP4_PSEUDO_HEADER, *PTCP_IP4_PSEUDO_HEADER;

typedef struct _TCP_IP6_PSEUDO_HEADER : public TCP_PSEUDO_HEADER{
	IPV6_ADDR		sourceip;
	IPV6_ADDR		destip;
} TCP_IP6_PSEUDO_HEADER, *PTCP_IP6_PSEUDO_HEADER;
#pragma pack ()

//
// TCP packet class
//

class TcpPacket : public Packet {
public:
	TcpPacket(void);
	~TcpPacket(void);

	bool			parsePacket		();
	void			assemblePacket		();
	string			toString		();
	uint32_t		getMinProtocolSize	();

	uint16_t		getSourceport		();
	uint16_t		getDestport		();
	uint32_t		getSequencenum		();
	uint32_t		getAcknum		();
	uint16_t		getHeaderlength		();
	uint16_t		getFlags		();
	uint16_t		getWindowsize		();
	uint16_t		getChecksum		();
	uint16_t		getUrgentpointer	();
	Packet::PAYLOAD_BUFFER	getOptions		();

	void			setSourceport		(uint16_t sourcep	);	
	void			setDestport		(uint16_t destp	);	
	void			setSequencenum		(uint32_t seqnum	);	
	void			setAcknum		(uint32_t aknum	);	
	void			setHeaderlength		(uint16_t hdlen	);	
	void			setFlags		(uint16_t flgs	);	
	void			setWindowsize		(uint16_t wsize	);	
	void			setChecksum		(uint16_t cksum	);	
	void			setUrgentpointer	(uint16_t urgpnt	);
	void			setOptions		(uint8_t* buf, int len);

	void			setIpAddresses		(IP_ADDR source, IP_ADDR dest);
	void			setIpAddresses		(IPV6_ADDR source, IPV6_ADDR dest);

	static AnonPrimitive*	anonSourceport;
	static AnonPrimitive*	anonDestport;
	static AnonPrimitive*	anonSeqnum;
	static AnonPrimitive*	anonAcknum;
	static AnonPrimitive*	anonFlags;
	static AnonPrimitive*	anonWindowsize;
	static AnonPrimitive*	anonUrgentpnt;
	static AnonPrimitive*	anonOptions;

private:
	TCP_HEADER		header;

	// the tcp options
	Packet::PAYLOAD_BUFFER	options;	// configuration item TcpOptions

	// the source and dest IP, we need this to calculate the checksum
	IP_ADDR			sourceip, destip;
	IPV6_ADDR		sourceip6, destip6;
	Packet::PROTOCOL	ipProtocol;

	// calculate the TCP checksum
	static uint16_t	checksum(PTCP_HEADER header, void* sourceip, void* destip, Packet::PROTOCOL ipproto, uint8_t* options, int optionslen, uint8_t* data, int datalen);
};

#endif // __TCP_PACKET_H
