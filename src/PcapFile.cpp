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

#include "PcapFile.h"
#include <cassert>

PcapFile::PcapFile(const string& file, Stream::RW_TYPE _rwtype)
: fullname (file), rwtype (_rwtype), stream (NULL)
{
	bool usestdin	= (file.compare ("stdin") == 0);
	bool usestdout	= (file.compare ("stdout") == 0);
	bool usefile	= ((usestdin | usestdout) == false);

	if (usestdin && (rwtype == Stream::RW_WRITE)) {
		cerr << "Invalid IO: stdin and write. Converting write to read" << std::endl;
		rwtype = Stream::RW_READ;
	}

	if (usestdout && (rwtype == Stream::RW_READ)) {
		cerr << "Invalid IO: stdout and read. Converting read to write" << std::endl;
		rwtype = Stream::RW_WRITE;
	}

	Stream::IO_SOURCE iosource = Stream::IO_FILE;

	if (usestdin)
		iosource = Stream::IO_CIN;
	else if (usestdout)
		iosource = Stream::IO_COUT;
	else if (usefile)
		iosource = Stream::IO_FILE;
	else 
		assert (false);

	if (usestdin | usestdout)
		stream = new Stream (iosource);
	else if (usefile)
		stream = new Stream (fullname, rwtype);
	else
		assert (false);

	assert (stream != NULL);

	if (! stream->good() || ! stream->isOpen ())
		cerr << "opening " << getFilename () << " failed" << std::endl;
}

PcapFile::~PcapFile(void)
{
	if (stream != NULL)
		delete stream;
}

string PcapFile::getFullname()
{
	return fullname;
}

string PcapFile::getFilename()
{
	string::size_type pos = fullname.find_last_of (SLASH);
	return fullname.substr (pos+1, fullname.length() - pos);
}

string PcapFile::getLocation() 
{
	string::size_type pos = fullname.find_last_of (SLASH);
	return fullname.substr (0, pos);
}

bool PcapFile::workHeader()
{
	if (! stream->good () || ! stream->isOpen()) {
		cerr << "opening " << getFilename() << " failed" << std::endl;
		return false;
	}

	if (rwtype == Stream::RW_READ) {
		
		//
		// read the pcap header
		//

		PCAP_FILE_HEADER header = {0};
		stream->read ((unsigned char*)&header, sizeof (PCAP_FILE_HEADER));
		if (! stream->good ()) {
			cerr << "reading pcap header from file " << getFilename() << " failed" << std::endl;
			return false;
		}

		//
		// check the magic code
		//

		if (header.magic != PCAP_MAGIC) {
			cerr 	<< "invalid magic code "
					<< header.magic
					<< " in file "
					<< getFilename()
					<< ", should be "
					<< PCAP_MAGIC
					<< std::endl;
			return false;
		}

		//
		// make sure this is an ethernet dump
		//

		if (header.network != LINKTYPE_ETHERNET) {
			cerr << "not an ethernet dump file" << std::endl;
			return false;
		} else {
			Frame::nettype = Frame::LINK_ETHERNET;
		}

		//cerr	<< "Pcap file "			<< getFilename()										<< std::endl
		//		<< "\tpcap version "	<< header.version_major << "." << header.version_minor	<< std::endl
		//		<< "\tnetwork type "	<< header.network										<< std::endl;

	} // if (type == FILE_TYPE::TYPE_READ)

	if (rwtype == Stream::RW_WRITE) {
		
		//
		// write the Pcap header
		//
		
		PCAP_FILE_HEADER header = {0};

		header.magic			= PCAP_MAGIC;
		header.network			= LINKTYPE_ETHERNET;
		header.sigfigs			= 0;
		header.snaplen			= 65535;	 
		header.thiszone			= 0; 
		header.version_major	= 2;
		header.version_minor	= 4;
	
		stream->write ((unsigned char*)&header, sizeof (PCAP_FILE_HEADER));

		if (! stream->good ()) {
			cerr << "writing pcap header to file " << getFilename() << " failed" << std::endl;
			return false;
		}	
	}

	return stream->good ();
}

Frame* PcapFile::readFrame ()
{
	if (! isOpen()) return NULL;
	bool suc = true;

	//
	// read the header of the packet header record
	//

	PCAP_REC_HEADER recheader = {{0}};
	suc &= stream->read ((unsigned char*)&recheader, sizeof (PCAP_REC_HEADER)*1);
	
	if (stream->eof ()) {
		//cerr << "reached end of file" << std::endl;
		return NULL;
	}

	if (! stream->good () || ! suc) {
		cerr << "reading record header failed" << std::endl;
		return NULL;
	}

	//
	// read the actual frame
	//

	Frame* ret = PacketFactory::instance()->createFrame ();
	
	ret->timestamp		= recheader.ts;
	ret->capturelength	= recheader.incl_len;
	ret->packetlength	= recheader.orig_len;

	if (ret->payloadpacket == NULL) {
		cerr << "not a known link type" << std::endl;
		PacketFactory::instance()->freeFrame (ret); 
		return NULL;
	}
	
	if (recheader.incl_len == 0 || recheader.incl_len > 65535) {
		cerr << "invalid frame size of " << recheader.incl_len << std::endl;
		PacketFactory::instance()->freeFrame (ret); 
		return NULL;	
	}
		
	ret->payloadpacket->setSize (recheader.incl_len);
	suc &= stream->read ((unsigned char*) ret->payloadpacket->getBuffer(), ret->payloadpacket->getSize());

	if (stream->eof () || recheader.incl_len <= 0) {
		//cerr << "reached end of file" << std::endl;
		PacketFactory::instance()->freeFrame (ret);
		return NULL;
	}

	if (! stream->good () || ! suc) {
		cerr << "reading packet failed" << std::endl;
		PacketFactory::instance()->freeFrame (ret);
		return NULL;
	}

	if (recheader.incl_len < ret->payloadpacket->getMinProtocolSize ()) {
		PacketFactory::instance()->freeFrame (ret);
		return NULL;
	}
	
	return ret;
}

bool PcapFile::writeFrame (Frame& pkt)
{
	if (! isOpen()) return false;
	bool ret = true;

	Packet::PAYLOAD_BUFFER payload = pkt.getPayloadBuffer ();

	//
	// write the record header
	// 

	PCAP_REC_HEADER header;
	header.ts		= pkt.timestamp;
	header.incl_len = payload.size;
	header.orig_len = payload.size;

	ret &= stream->write ((unsigned char*)&header, sizeof (PCAP_REC_HEADER));

	//
	// write the packet
	//

	ret &= stream->write ((unsigned char*)payload.buf, payload.size);
	payload.destroy();
		
	//
	// check for consistency
	//

	if (! stream->good () || ! ret) {
		cerr << "writing packet failed" << std::endl;
		return false;
	}

	return true;
}

bool PcapFile::isOpen ()
{
	return stream->isOpen ();
}

Stream&	PcapFile::getStream	()
{
	return *stream;
}

