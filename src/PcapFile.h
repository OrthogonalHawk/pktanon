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

#ifndef __PCAP_FILE_H
#define __PCAP_FILE_H

#include <string>
#include <iostream>
#include "packets/Frame.h"
#include "packets/PacketFactory.h"
#include "Stream.h"

using std::string;
using std::basic_string;
using std::cerr;

#ifdef WIN32
	#define		SLASH	"\\"	
#else
	#define		SLASH	"/"
#endif

#define PCAP_MAGIC					0xa1b2c3d4
#define LINKTYPE_ETHERNET			1	// currently only support ethernet dumps

class PcapFile
{
public:

	PcapFile												(const string& file, Stream::RW_TYPE tp);
	~PcapFile												(void);

	string					getFullname						();
	string					getFilename						();
	string					getLocation						();

	bool					workHeader						();
	bool					isOpen							();

	Frame*					readFrame						();
	bool					writeFrame						(Frame& pkt);

	Stream&					getStream						();

private:
	string					fullname;
	Stream*					stream;
	Stream::RW_TYPE			rwtype;

	#pragma pack (1)
	typedef struct _PCAP_FILE_HEADER {
		uint32_t	magic;				// always 0xa1b2c3d4
		uint16_t	version_major;		// 2 for now
		uint16_t	version_minor;		// 4 for now
		uint32_t	thiszone;			// GMT to local correction
		uint32_t	sigfigs;			// accuracy of timestamps, 0 is fine
		uint32_t	snaplen;			// max length of captured packets
		uint32_t	network;			// data link type
	} PCAP_FILE_HEADER, *PPCAP_FILE_HEADER;
	#pragma pack ()

	#pragma pack (1)
	typedef struct _PCAP_REC_HEADER {
		Frame::TIME_VAL	ts;				// timestamp
		uint32_t	incl_len;			// number of octets of packet saved in file
		uint32_t	orig_len;			// actual length of packet
	} PCAP_REC_HEADER, *PPCAP_REC_HEADER;
	#pragma pack ()

};

#endif // __PCAP_FILE_H
