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

#ifndef __PACKET_FACTORY_H
#define __PACKET_FACTORY_H

#include "Packet.h"
#include "EthernetPacket.h"
#include "IpPacket.h"
#include "TcpPacket.h"
#include "UdpPacket.h"
#include "IcmpPacket.h"
#include "PayloadPacket.h"
#include "ArpPacket.h"
#include "Ipv6Packet.h"
#include "VlanPacket.h"
#include "Frame.h"

//
// this flag control whether the packetfactory
// uses the boost::pool lib or normal new and free
// memory allocation
//
// #define PACKET_FACTORY_NORMAL_ALLOCATION	1
//

#ifdef _CRTDBG_MAP_ALLOC
	// boost::pool does not work with memory debugging
	#undef PACKET_FACTORY_NORMAL_ALLOCATION	
#endif 

#ifndef PACKET_FACTORY_NORMAL_ALLOCATION
	#include "boost/pool/poolfwd.hpp"
	#include "boost/pool/object_pool.hpp"
#endif

class PacketFactory {
public:
	static PacketFactory*	instance	();

	Packet*			createPacket	(Packet::PROTOCOL protocol);
	Frame*			createFrame	();

	void			freePacket	(Packet* packet);
	void			freeFrame	(Frame* frame);

private:

	PacketFactory();
	~PacketFactory();

	class Guard {
		public: ~Guard() {
			if (PacketFactory::object != NULL)
				delete PacketFactory::object;
				PacketFactory::object = NULL;
			}
	};

	friend class		Guard;
	static PacketFactory*	object;

#ifndef PACKET_FACTORY_NORMAL_ALLOCATION

	boost::object_pool <Frame>		poolFrame;
	boost::object_pool <EthernetPacket>	poolEthernetPacket;
	boost::object_pool <ArpPacket>		poolArpPacket;
	boost::object_pool <IpPacket>		poolIpPacket;
	boost::object_pool <Ipv6Packet>		poolIpv6Packet;
	boost::object_pool <IcmpPacket>		poolIcmpPacket;
	boost::object_pool <UdpPacket>		poolUdpPacket;
	boost::object_pool <TcpPacket>		poolTcpPacket;
	boost::object_pool <VlanPacket>		poolVlanPacket;
	boost::object_pool <PayloadPacket>	poolPayloadPacket;

#endif
};

#endif // __PACKET_FACTORY_H
