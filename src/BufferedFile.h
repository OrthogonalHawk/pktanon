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

#ifndef __BUFFERED_FILE_H
#define __BUFFERED_FILE_H

#include <fstream>
#include <ostream>
#include <istream>
#include <iostream>
#include <cassert>
#include <ios>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdint.h>

using std::char_traits;
using std::basic_istream;
using std::basic_ostream;
using std::fstream;
using std::streamsize;
using std::ios_base;
using std::string;

#define READ_BLOCK_SIZE		1024	// default (overwritten using setReadSize)  block size for reads  (in bytes)
#define WRITE_BLOCK_SIZE	1024	// default (overwritten using setWriteSize) block size for writes (in bytes)

class BufferedFile
{
public:
											BufferedFile	();
											~BufferedFile	();

	void									write			(const fstream::char_type* _Str, streamsize _Count);
	void									read			(fstream::char_type* _Str, streamsize _Count);
	void									open			(const string& filename, ios_base::openmode mode);
	streamsize								gcount			() const;
	bool									eof				() const;
	void									close			();
	bool									good			() const;
	bool									is_open			() const;
	void									flush			();

	void									setReadSize		(unsigned int size);
	void									setWriteSize	(unsigned int size);

private:

	char*									readbuffer;
	unsigned int							readbuffersize;
	unsigned int							readbufferitems_low;
	unsigned int							readbufferitems_high;
	streamsize								lastreadcount;

	char*									writebuffer;
	unsigned int							writebuffersize;
	unsigned int							writebufferitems;

	fstream*								internalstream;
};

#endif // __BUFFERED_FILE_H
