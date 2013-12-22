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

#ifndef __ANON_BYTEWISE_HASH_HMAC_SHA1_H
#define __ANON_BYTEWISE_HASH_HMAC_SHA1_H

#include "AnonBytewise.h"
#include "AnonHashHmacSha1.h"

class AnonBytewiseHashHmacSha1 : public AnonBytewise {

public:
							AnonBytewiseHashHmacSha1		(string key);
							~AnonBytewiseHashHmacSha1		();

private:

	void					fillTable						(string hmackey);

};


#endif // __ANON_BYTEWISE_HASH_HMAC_SHA1_H 

