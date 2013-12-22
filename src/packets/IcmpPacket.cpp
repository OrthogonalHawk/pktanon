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

#include "IcmpPacket.h"

AnonPrimitive* IcmpPacket::anonType = NULL;
AnonPrimitive* IcmpPacket::anonCode = NULL;
AnonPrimitive* IcmpPacket::anonMisc = NULL;

IcmpPacket::IcmpPacket(void)
{
	memset (&header, 0, sizeof (ICMP_HEADER));
	protocol = Packet::PROTO_ICMP;
}

IcmpPacket::~IcmpPacket(void)
{
}

bool IcmpPacket::parsePacket()
{
	memcpy		(&header, buffer, sizeof (ICMP_HEADER)); 
	layersize	= sizeof (ICMP_HEADER);

	if (Configuration::instance()->getSetBadChecksumsToBad ()) {

		uint16_t chksum	= header.checksum;
		header.checksum		= 0;
		this->checksumgood	= (checksum(&header, getBuffer() + layersize, getSize() - layersize) == chksum);
		header.checksum		= chksum;

	} // if (Configuration::instance()->getSetBadChecksumsToBad ()) 

	header.checksum	= swap16 (header.checksum);

	//
	// depending on the type and code
	// we may have an IP Packet in the data
	//

	nextProtocol = Packet::PROTO_DATA_PAYLOAD;

	switch (header.type) {

		case 3:		// destination unreachable message 
		case 4:		// source quench message
		case 5:		// redirect message
		case 11:	// time exceeded message
		case 12:	// parameter problem message
			{
				nextProtocol = Packet::PROTO_IP;
				break;
			}
		case 0:		// echo message
		case 8:		// echo reply message
		case 13:	// timestamp
		case 14:	// timestamp reply
			{
				nextProtocol = Packet::PROTO_DATA_PAYLOAD;
				break;
			}
		case 15:	// information Request 
		case 16:	// information Reply 
		default:
			{
				nextProtocol = Packet::PROTO_NONE;
				break;
			}
	} // switch (header.type) 
	
	return true;
}

uint8_t IcmpPacket::getType ()
{
	return header.type;
}

uint8_t IcmpPacket::getCode ()
{
	return header.code;
}

void IcmpPacket::setType (uint8_t tp)
{
	header.type = tp;
}

void IcmpPacket::setCode (uint8_t cd)
{
	header.code = cd;
}

uint16_t IcmpPacket::getChecksum ()
{
	return header.checksum;
}

void IcmpPacket::setChecksum (uint16_t chk)
{
	header.checksum = chk;
}

void IcmpPacket::assemblePacket ()
{
	if (nextPacket != NULL)
		nextPacket->assemblePacket ();

	int thissize = sizeof(ICMP_HEADER);
	int nextsize = nextPacket != NULL ? nextPacket->getSize() : 0;

	setSize (thissize + nextsize);

	if (nextPacket != NULL)
		memcpy (buffer + thissize, nextPacket->getBuffer(), nextsize);

	header.checksum	= swap16 (header.checksum);

	//
	// calc new checksum
	//

	setChecksum (0);

	if (Configuration::instance()->getReCalcChecksums ()) {

		if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood) {

			setChecksum (RandomNumberGenerator::generate ());

		} else {

			uint8_t*	data	= NULL;
			uint32_t	datalen = 0;

			if (nextProtocol != Packet::PROTO_NONE && nextPacket != NULL) {
				data	= nextPacket->getBuffer ();
				datalen = nextPacket->getSize	();
			}

			setChecksum		(checksum (&header, data, datalen));

		} // if (Configuration::instance()->getSetBadChecksumsToBad () && !checksumgood)

	} // if (Configuration::instance()->getReCalcChecksums ()) 

	//
	// copy into buffer
	//

	memcpy (buffer, &header, sizeof(ICMP_HEADER));
	header.checksum	= swap16 (header.checksum);
}

uint16_t IcmpPacket::checksum (PICMP_HEADER header, uint8_t* data, int datalen)
{
	uint8_t* buffer = (uint8_t*) malloc (sizeof (ICMP_HEADER) + datalen);
	memcpy (buffer, header, sizeof (ICMP_HEADER));
	memcpy (buffer + sizeof (ICMP_HEADER), data, datalen);

	uint16_t sum = Packet::checksum (buffer, sizeof (ICMP_HEADER) + datalen);
	free (buffer);

	return sum;
}

string IcmpPacket::toString ()
{
	ostringstream out;

	out << "ICMP packet"		<< std::endl
		<< "\ttype: \t"		<< (uint32_t)getType() << std::endl
		<< "\tcode: \t"		<< (uint32_t)getCode() << std::endl
		<< "\tchecksum: \t0x"	<< std::hex << std::setw (4) << std::setfill ('0') << getChecksum() << std::endl << std::dec;

	return out.str ();
}

uint32_t IcmpPacket::getMisc ()
{
	return header.misc;
}

void IcmpPacket::setMisc (uint32_t msc) 
{
	header.misc = msc;
}

uint32_t IcmpPacket::getMinProtocolSize ()
{
	return sizeof (ICMP_HEADER);
}
