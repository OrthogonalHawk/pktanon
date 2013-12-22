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

#include "Utils.h"

string Utils::trim (string str)
{
	string ret = str;

	ret.erase (0, ret.find_first_not_of( " \n\r\t" ));
	ret.erase (ret.find_last_not_of( " \n\r\t") + 1 );

	return ret;
}

Utils::STRING_LIST	Utils::split (string str, string delimiter)
{
	STRING_LIST			ret;
	string::size_type	offset		= 0;
	string::size_type	delimIndex	= 0;
	string				helpstring	= "";
    
	if (str.length () <= 0) return ret;

    while ((delimIndex = str.find (delimiter, offset)) != string::npos) {
        
		helpstring = trim (str.substr (offset, delimIndex - offset));
		if (helpstring.length () > 0) ret.push_back	(helpstring);

		offset			= delimIndex + delimiter.length();
        delimIndex		= str.find (delimiter, offset);

    } // while ((delimIndex = str.find (delimiter, offset)) != string::npos) 

	if (offset < str.length ()) {
		helpstring = trim (str.substr (offset));
		if (helpstring.length () > 0) ret.push_back (helpstring);
	}

	return ret;
}
