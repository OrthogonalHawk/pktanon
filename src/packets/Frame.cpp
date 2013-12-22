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

#include "Frame.h"
#include <cassert>
#include "packets/PacketFactory.h"

Frame::LINK_TYPE Frame::nettype = Frame::LINK_UNKNOWN;

Frame::Frame(void) 
{
	switch (nettype) {
		case Frame::LINK_ETHERNET:
			payloadpacket = PacketFactory::instance()->createPacket(Packet::PROTO_ETHERNET);
			break;

		case Frame::LINK_UNKNOWN:
		default:
			cerr << "invalid network type" << std::endl;
			payloadpacket = NULL;
	} // switch (nettype)
}

Frame::~Frame(void)
{
	PacketFactory::instance()->freePacket (payloadpacket);
}

Packet::PAYLOAD_BUFFER Frame::getPayloadBuffer()
{
	payloadpacket->assemblePacket ();

	Packet::PAYLOAD_BUFFER ret;
	
	ret.size = payloadpacket->getSize();
	ret.buf = (uint8_t*) malloc (ret.size);

	memcpy (ret.buf, payloadpacket->getBuffer(), ret.size);

	return ret;
}

string Frame::toString()
{
	ostringstream out;

	out << "Frame"			<< std::endl
		<< "\tnettype: \t"	<< nettype << std::endl
		<< "\ttimestamp: \t"	<< timestamp.toString() << std::endl
		<< "\tlength: \t"	<< capturelength << " captured, "
					<< packetlength	 << " original" << std::endl;
	
	Packet* cur = payloadpacket;

	while (cur != NULL) {
		out << cur->toString() << std::endl;
		cur = cur->getNextPacket ();
	}
		
	out << std::endl;
	return out.str ();
}
