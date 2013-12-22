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

#include "PacketFactory.h"

PacketFactory* PacketFactory::object = NULL;

PacketFactory* PacketFactory::instance ()
{
	static Guard guard;

	if (object == NULL)
		object = new PacketFactory ();
	
	return object;
}

PacketFactory::PacketFactory ()
{
}

PacketFactory::~PacketFactory ()
{
}

Packet* PacketFactory::createPacket (Packet::PROTOCOL protocol)
{
	Packet* ret = NULL;

	switch (protocol) {
		case Packet::PROTO_ETHERNET: 
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new EthernetPacket ();
#else
				ret = poolEthernetPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_ARP:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new ArpPacket ();
#else
				ret = poolArpPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_ICMP:	
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new IcmpPacket ();
#else
				ret = poolIcmpPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_IP:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new IpPacket ();
#else	
				ret = poolIpPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_IPV6:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new Ipv6Packet ();
#else
				ret = poolIpv6Packet.construct (); 
#endif
				break;
			}
		case Packet::PROTO_TCP:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new TcpPacket ();
#else
				ret = poolTcpPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_UDP:
			{	
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new UdpPacket ();
#else
				ret = poolUdpPacket.construct ();
#endif
				break;
			}

		case Packet::PROTO_VLAN:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new VlanPacket ();
#else
				ret = poolVlanPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_DATA_PAYLOAD:
			{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
				ret = new PayloadPacket ();
#else
				ret = poolPayloadPacket.construct ();
#endif
				break;
			}
		case Packet::PROTO_NONE:
		default:
			{
				ret = NULL;
				break;
			}
	} // switch (nextProtocol) 

	return ret;
}

Frame* PacketFactory::createFrame ()
{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
	return new Frame ();
#else
	return poolFrame.construct ();
#endif
}

void PacketFactory::freePacket (Packet* packet)
{
	if (packet == NULL) return;

#ifdef PACKET_FACTORY_NORMAL_ALLOCATION

	delete packet;

#else

	Packet::PROTOCOL protocol = packet->getProtocol ();
	switch (protocol) {
		case Packet::PROTO_ETHERNET:	poolEthernetPacket.destroy	((EthernetPacket*)	packet); 	break;
		case Packet::PROTO_ARP:			poolArpPacket.destroy		((ArpPacket*)		packet); 	break;
		case Packet::PROTO_ICMP:		poolIcmpPacket.destroy		((IcmpPacket*)		packet); 	break;
		case Packet::PROTO_IP:			poolIpPacket.destroy		((IpPacket*)		packet); 	break;
		case Packet::PROTO_IPV6:		poolIpv6Packet.destroy		((Ipv6Packet*)		packet); 	break;
		case Packet::PROTO_TCP:			poolTcpPacket.destroy		((TcpPacket*)		packet); 	break;
		case Packet::PROTO_UDP:			poolUdpPacket.destroy		((UdpPacket*)		packet); 	break;
		case Packet::PROTO_VLAN:		poolVlanPacket.destroy		((VlanPacket*)		packet);	break;
		case Packet::PROTO_DATA_PAYLOAD:poolPayloadPacket.destroy	((PayloadPacket*)	packet); 	break;
		default:						assert (false); free (packet);								break;
	} // switch (nextProtocol) 

#endif
}

void PacketFactory::freeFrame (Frame* frame)
{
#ifdef PACKET_FACTORY_NORMAL_ALLOCATION
	delete frame;
#else
	poolFrame.destroy (frame);
#endif
}
