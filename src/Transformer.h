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

#ifndef __TRANSFORMER_H
#define __TRANSFORMER_H

#include <map>
#include <bitset>
#include <fstream>
#include <ios>

#include "packets/Packet.h"
#include "packets/EthernetPacket.h"
#include "packets/IpPacket.h"
#include "packets/IcmpPacket.h"
#include "packets/UdpPacket.h"
#include "packets/TcpPacket.h"
#include "packets/PayloadPacket.h"
#include "packets/ArpPacket.h"
#include "packets/Ipv6Packet.h"
#include "packets/PacketFactory.h"

using std::map;
using std::pair;
using std::bitset;
using std::ifstream;
using std::ios_base;

//
// class to transform packets 
//

class Transformer
{
public:
	Transformer										(void);
	~Transformer									(void);

	Packet*					getTransformedPacket	(Packet& inpacket);
	void					setSha1Key				(const char* buf, int len);
	void					config					(bool _bytewiseip, bool _bytewisemac, bool _ipcidr);

private:

	void					transformPacket			(EthernetPacket&	inpkt, EthernetPacket&	ret);
	void					transformPacket			(ArpPacket&			inpkt, ArpPacket&		ret);
	void					transformPacket			(IpPacket&			inpkt, IpPacket&		ret);
	void					transformPacket			(IcmpPacket&		inpkt, IcmpPacket&		ret);
	void					transformPacket			(TcpPacket&			inpkt, TcpPacket&		ret);
	void					transformPacket			(UdpPacket&			inpkt, UdpPacket&		ret);
	void					transformPacket			(PayloadPacket&		inpkt, PayloadPacket&	ret);
	void					transformPacket			(Ipv6Packet&		inpkt, Ipv6Packet&		ret);
	void					transformPacket			(VlanPacket&		inpkt, VlanPacket&		ret);

};

#endif // __TRANSFORMER_H
