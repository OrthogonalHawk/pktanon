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

#ifndef __STREAM_H
#define __STREAM_H

#include "BufferedFile.h"
#include "Configuration.h"
#include <cassert>
#include <string>
#include <ios>
#include <iostream>
#include <fcntl.h>

#ifdef WIN32
	#include <io.h>	
#endif

using std::cerr;
using std::ios_base;
using std::string;

class Stream
{
public:

	typedef enum _IO_SOURCE {
		IO_FILE,
		IO_CIN,
		IO_COUT,
	} IO_SOURCE; 

	typedef enum _RW_TYPE {
		RW_READ		= 1,
		RW_WRITE	= 2,
	} RW_TYPE; 

	Stream	(const string& _filename, Stream::RW_TYPE _type);
	Stream	(Stream::IO_SOURCE _iotype);
	~Stream	(void);

	bool	read	(unsigned char* buf, int count);
	bool	write	(unsigned char* buf, int count);
	
	bool	good	();
	bool	eof		();
	bool	isOpen	();
	bool	flush	();

	Stream::IO_SOURCE	getIoSource			();
	Stream::RW_TYPE		getReadWriteType	();

private:

	Stream::IO_SOURCE	iosource;
	Stream::RW_TYPE		rwtype;

	BufferedFile*		fileobj;
};


#endif // __STREAM_H

