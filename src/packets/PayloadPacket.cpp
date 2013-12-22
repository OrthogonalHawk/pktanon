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

#include "PayloadPacket.h"

AnonPrimitive* PayloadPacket::anonPayload = NULL;

PayloadPacket::PayloadPacket()
{
	protocol = Packet::PROTO_DATA_PAYLOAD;
}

PayloadPacket::~PayloadPacket(void)
{
	data.destroy ();
}

bool PayloadPacket::parsePacket ()
{
	data.size = getSize ();
	data.buf = (uint8_t*) malloc (data.size);

	memcpy (data.buf, getBuffer(), data.size);

	nextProtocol	= Packet::PROTO_NONE;
	layersize	= getSize ();

	return true;
}

void PayloadPacket::assemblePacket ()
{
	setSize (data.size);
	memcpy (getBuffer(), data.buf, data.size);
}

Packet::PAYLOAD_BUFFER PayloadPacket::getData ()
{
	Packet::PAYLOAD_BUFFER retbuf;

	retbuf.buf = (uint8_t*) malloc (data.size);
	retbuf.size = data.size;

	memcpy (retbuf.buf, data.buf, data.size);

	// TODO:	return pseudo size
	//			need adaption in all procols that have
	//			a length field: IP, UDP (?), IPv6, more?

	return retbuf;
}

void PayloadPacket::setData (uint8_t* buf, int len)
{
	data.buf = (uint8_t*) realloc (data.buf, len);
	data.size = len;

	memcpy (data.buf, buf, len);
}

string PayloadPacket::toString () 
{
	ostringstream out;
	out << "Payload packet"		<< std::endl
		<< "\tlength: \t"	<< data.size			<< std::endl
		<< "\tdata: \t\t"	<< data.toString (false)	<< std::endl
		<< "\tstring: \t\""	<< data.toString (true)		<< "\"";

	return out.str ();
}

uint32_t PayloadPacket::getMinProtocolSize()
{
	return 1; // the protocol needs to have at least ony byte
}
