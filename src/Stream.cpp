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

#include "Stream.h"

Stream::Stream (const string& _filename, Stream::RW_TYPE _type)
:	iosource	(Stream::IO_FILE), 
	rwtype		(_type),
	fileobj		(NULL)
{
	fileobj = new BufferedFile();

	//
	// set block size for read and write operations
	//

	unsigned long readsize  = Configuration::instance()->getReadBlockSize();
	unsigned long writesize = Configuration::instance()->getReadBlockSize();
	
	//if (readsize <= 0 && iosource == Stream::IO_FILE && rwtype == Stream::RW_READ) 
	//	cerr << _filename << ": buffered blockwise HD reading disabled" << std::endl;

	//if (writesize <= 0 && iosource == Stream::IO_FILE && rwtype == Stream::RW_WRITE) 
	//	cerr << _filename << ": buffered blockwise HD writing disabled" << std::endl;

	fileobj->setReadSize  (readsize);
	fileobj->setWriteSize (writesize);

	//
	// open file
	//

	ios_base::openmode mode = ios_base::binary;
	if (rwtype == Stream::RW_WRITE)	mode |= ios_base::trunc | ios_base::out;
	if (rwtype == Stream::RW_READ)	mode |= ios_base::in;

	fileobj->open (_filename, mode);
}

Stream::Stream (Stream::IO_SOURCE _iosource) : iosource (_iosource)
{
	fileobj = new BufferedFile();
	assert (iosource != Stream::IO_FILE);
	rwtype = (iosource == Stream::IO_CIN ? Stream::RW_READ : Stream::RW_WRITE);

#ifdef WIN32
	_setmode (_fileno (iosource == Stream::IO_CIN ? stdin : stdout), O_BINARY);
#endif

#ifdef LINUX
	if (iosource == Stream::IO_CIN) {
		int fd	= fileno (stdin);
		int val = fcntl(fd, F_GETFL, 0);
		if (fcntl (fd, F_SETFL, val & ~O_NONBLOCK) == -1) {
			cerr << "setting terminal nonblocking mode failed" << std::endl;
			exit (0);
		}
	}
#endif

	// fread and fwrite work automatically with 
	// no need to open/close the object
}

Stream::~Stream(void)
{
	if (iosource == Stream::IO_FILE && isOpen())
		fileobj->close ();
	delete fileobj;
	fileobj = NULL;
}

bool Stream::read (unsigned char* buf, int count)
{
	if (! isOpen()) return false;
	size_t read = 0;

	switch (iosource) {
		case Stream::IO_FILE:
			{
				fileobj->read ((fstream::char_type*)buf, count);
				break;
			}
		case Stream::IO_CIN:
			{
				// TODO: needed? when in online mode it only works to Strg+C without the two lines
				// while (read != count * 1) {
					read += fread (buf + read, 1, count - read, stdin);
				//}
				break;
			}
		case Stream::IO_COUT:
			{
				return false;
			}
		default: 
			{
				return false;
			}
	}

	return good ();
}

bool Stream::write (unsigned char* buf, int count)
{
	if (! isOpen()) return false;
	size_t written = 0;

	switch (iosource) {
		case Stream::IO_FILE:
			{
				fileobj->write ((fstream::char_type*)buf, count);
				break;
			}
		case Stream::IO_CIN:
			{
				return false;
			}
		case Stream::IO_COUT:
			{
				written = fwrite (buf, 1, count, stdout);
				if (written != count * 1) return false;

				break;
			}
		default: 
			{
				return false;
			}
	}

	return good ();
}

bool Stream::good ()
{
	if (! isOpen()) return false;

	return (iosource == Stream::IO_FILE) ? fileobj->good () : true;
}

bool Stream::eof ()
{
	if (! isOpen()) return true;

	return (iosource == Stream::IO_FILE) ? fileobj->eof () : false;
}

bool Stream::isOpen	()
{
	return (iosource == Stream::IO_FILE) ? fileobj->is_open () : true;
}

bool Stream::flush ()
{
	//
	// can only flush the standard in buffer
	//

	if (iosource == Stream::IO_CIN) {
		fileobj->flush ();
		return true;
	} else {
		return false;
	}
}

Stream::IO_SOURCE Stream::getIoSource ()
{
	return iosource;
}

Stream::RW_TYPE Stream::getReadWriteType ()
{
	return rwtype;
}
