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

#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <list>

using std::list;
using std::string;

namespace Utils {
	
	/// trim string
	string							trim		(string	str);

	/// split a string into substrings. The single strings trimmed from whitespace
	/// only strings that have a resulting length > 0 after the trim are in the list
	typedef list<string>			STRING_LIST;
	typedef STRING_LIST::iterator	STRING_LIST_ITERATOR;
	STRING_LIST						split		(string	str, string delimiter);

#ifdef WIN32
	const string DIRECTORY_DELIMITER = "\\";
#else
	const string DIRECTORY_DELIMITER = "/";
#endif

};

#endif // __UTILS_H
