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

#include "BufferedFile.h"

BufferedFile::BufferedFile ()
:	readbuffersize			(READ_BLOCK_SIZE),
	writebuffersize			(WRITE_BLOCK_SIZE),
	readbufferitems_low		(0),
	readbufferitems_high	(0),
	writebufferitems		(0),
	lastreadcount			(0),
	internalstream			(NULL)
{
	readbuffer	= (char*) malloc (readbuffersize);
	writebuffer	= (char*) malloc (writebuffersize);
}

BufferedFile::~BufferedFile ()
{
	close ();

	free (readbuffer);
	free (writebuffer);
}

void BufferedFile::open(const string& filename, ios_base::openmode mode)
{
	internalstream = new fstream();
	internalstream->open(filename.c_str(), mode);
}

bool BufferedFile::good() const
{
	return internalstream->good();
}

bool BufferedFile::is_open() const
{
	return internalstream->is_open();
}

void BufferedFile::flush()
{
	internalstream->flush();
}

void BufferedFile::setReadSize (unsigned int size)
{
	//
	// settings the block size to 0 will disable
	// the bufferedfile and pass all reads directly to fstream
	//

	readbuffersize = size;
	readbuffer = (char*) realloc (readbuffer, readbuffersize);
}

void BufferedFile::setWriteSize	(unsigned int size)
{
	//
	// settings the block size to 0 will disable
	// the bufferedfile and pass all writes directly to fstream
	//

	writebuffersize = size;
	writebuffer	= (char*) realloc (writebuffer, writebuffersize);
}

void BufferedFile::close ()
{
	if(internalstream == NULL) return;

	//
	// write the rest of the writecache and close the stream. 
	// No action needed for the read buffer
	//

	if (writebufferitems > 0) {
		internalstream->write (writebuffer, writebufferitems);
		writebufferitems = 0;
	}

	internalstream->close ();
	delete internalstream;
	internalstream = NULL;
}

void BufferedFile::write (const fstream::char_type* _Str, streamsize _Count)
{
	//
	// is bufferedfile switched on? if not, pass to fstream
	//

	if (writebuffersize == 0){
		internalstream->write (_Str, _Count);
		return;
	}

	assert (writebufferitems <= writebuffersize);
	
	//
	// is any space in the buffer for the chunk 
	// or do we have to write some stuff to disc?
	//

	if (_Count > (int)(writebuffersize - writebufferitems)) {
		
		//
		// not enough space in the buffer
		//
		// fill the buffer until it is full, then write it to disc
		//

		unsigned int bufferspace = writebuffersize - writebufferitems;
		assert (bufferspace >= 0);

		memcpy (writebuffer + writebufferitems, _Str, bufferspace);
		writebufferitems += bufferspace;
		assert (writebufferitems == writebuffersize);

		internalstream->write (writebuffer, writebuffersize);
		writebufferitems = 0;
	
		//
		// is the remaining data is a multiple of the buffer size
		// write it to disc in chunks that have the size of the buffer
		//

		unsigned int strpos = bufferspace;
		assert ((int)bufferspace < _Count);
		
		while (((_Count - strpos) / writebuffersize) > 0) {

			internalstream->write (_Str + strpos, writebuffersize);
			strpos += writebuffersize;
				
		}

		assert (_Count - strpos >= 0);

		//
		// copy the remaining data into the buffer
		// no items are currently in the buffer
		// they have been written above to the fstream
		//

		memcpy (writebuffer, _Str + strpos, _Count - strpos);
		writebufferitems = (_Count - strpos);

	} else {

		//
		// enough space in the buffer
		//
		// write the data into the buffer
		// there is still enough space left
		//

		memcpy (writebuffer + writebufferitems, _Str, _Count);
		writebufferitems += _Count;

	}

	assert (writebufferitems <= writebuffersize);
}

void BufferedFile::read (fstream::char_type* _Str, streamsize _Count)
{
	//
	// if bufferedfile is switched of pass read to fstream
	//

	if (readbuffersize == 0){
		internalstream->read (_Str, _Count);
		return;
	}

	assert (readbufferitems_low		<= readbufferitems_high	);
	assert (readbufferitems_high	<= readbuffersize		);

	//
	// do we have enough data to satisfy the read request
	// or do we have to fetch some from the fstream first?
	//

	do {

		if (_Count > (int)(readbufferitems_high - readbufferitems_low)) {

			//
			// not enough data to satisfy the read request
			//
			// first copy the contents of the readbuffer into the return buffer
			// so that we have no more data available 
			//

			unsigned int available	= readbufferitems_high - readbufferitems_low;
			unsigned int strpos		= 0;
			memcpy (_Str + strpos, readbuffer + readbufferitems_low, available);

			strpos				  += available;
			readbufferitems_low	   = 0;
			readbufferitems_high   = 0;

			//
			// then see how many complete chunks with readbuffersize we have to read
			//

			unsigned int remainingrequest	= _Count - available;
			unsigned int readnum			= 0;
			assert (remainingrequest >= 0);

			while ((remainingrequest / readbuffersize) != 0) {
					
				internalstream->read (_Str + strpos, readbuffersize);
				readnum	= internalstream->gcount ();

				// not enough data available to read
				if (readnum < readbuffersize) {
					assert (internalstream->eof());

					readbufferitems_low	 = 0;
					readbufferitems_high = 0;
					lastreadcount		 = readnum;
					
					break;
				} // if (readnum < readbuffersize) 

				remainingrequest	-= readnum;	
				strpos				+= readnum;
			}
				
			//
			// now get the next chunk to get the remaining buffer
			//

			internalstream->read (readbuffer, readbuffersize);
			readbufferitems_low		= 0;
			readbufferitems_high	= internalstream->gcount();

			if (remainingrequest > (readbufferitems_high - readbufferitems_low))
				remainingrequest = (readbufferitems_high - readbufferitems_low);

			memcpy (_Str + strpos, readbuffer + readbufferitems_low, remainingrequest);
			readbufferitems_low += remainingrequest;
			lastreadcount		 = strpos + remainingrequest;

		} else {
			
			//
			// we have enough data to satisfy the read request
			// just return the items
			//

			memcpy (_Str, readbuffer + readbufferitems_low, _Count);
			readbufferitems_low += _Count;
			lastreadcount        = _Count;

		}

	} while (false);

	assert (readbufferitems_low		<= readbufferitems_high	);
	assert (readbufferitems_high	<= readbuffersize		);
}

streamsize BufferedFile::gcount () const
{
	if (readbuffersize == 0)	return internalstream->gcount ();
	else						return lastreadcount;
}

bool BufferedFile::eof () const
{
	//
	// eof can only be changed by read operations
	// so we only look at the readbuffer buffer
	//

	if (readbuffersize == 0)	
		return internalstream->eof ();
	else
		return (internalstream->eof() && (readbufferitems_low >= readbufferitems_high));
}
