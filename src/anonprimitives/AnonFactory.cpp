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

#include "./AnonFactory.h"

AnonFactory::AnonFactory ()
{
}

AnonFactory::~AnonFactory ()
{
}

AnonPrimitive* AnonFactory::create (Triggerconf::ATTRIBUTE_MAP attrmap)
{
	//
	// get the name attribute
	//

	Triggerconf::ATTRIBUTE_MAP_ITERATOR itname = attrmap.find (Configuration::ANON_ATTR_PRIMITIVE);
	string								name;
	AnonPrimitive*						primitive		= NULL;
	AnonPrimitive*						remainingchain	= NULL;

	if (itname != attrmap.end ())
		name = itname->second;
	else
		return NULL;

	//
	// create a single primitive or a chain?
	// if we build a chain, we will do this recursively
	//

	string::size_type firstpos;

	if ((firstpos = name.find_first_of (",")) != string::npos) {

 		string								remaining	= name.substr (firstpos + 1, name.length() - firstpos);
											name		= name.substr (0, firstpos);

		remaining	= Utils::trim (remaining);
		name		= Utils::trim (name);

		Triggerconf::ATTRIBUTE_MAP			mapcopy		(attrmap);
		Triggerconf::ATTRIBUTE_MAP_ITERATOR iname		=  mapcopy.find (Configuration::ANON_ATTR_PRIMITIVE);

		assert (iname != mapcopy.end ());

		iname->second	= remaining;
		remainingchain	= create (mapcopy);
		
	} // if ((firstpos = name.find_first_of (",")) != string::npos) 

	//
	// create the right primitive and set their parameters
	//

	if		  (name.compare ("AnonConstOverwrite")		== 0) {	
		
		Triggerconf::ATTRIBUTE_MAP_ITERATOR it		= attrmap.find ("anonval");
		unsigned char						byte	= 0x00;
		string								sbyte;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonConstOverwrite: missing anonval attribute" << std::endl;
			exit (0);
		}

		sbyte		= it->second;
		byte		= strtoul (sbyte.c_str (), NULL, 16);
		primitive	= new AnonConstOverwrite (byte);

	} else if (name.compare ("AnonContinuousChar")		== 0) {
		primitive = new AnonContinuousChar ();
	} else if (name.compare ("AnonHashSha1")			== 0) {
		primitive = new AnonHashSha1 ();
	} else if (name.compare ("AnonIdentity")			== 0) {
		primitive = new AnonIdentity ();
	} else if (name.compare ("AnonRandomize")			== 0) {
		primitive = new AnonRandomize ();
	} else if (name.compare ("AnonShorten")				== 0) {

		Triggerconf::ATTRIBUTE_MAP_ITERATOR it	= attrmap.find ("newlen");
		unsigned int						len;
		string								slen;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonShorten: missing newlen attribute" << std::endl;
			exit (0);
		}

		slen		= it->second;
		len			= strtoul (slen.c_str (), NULL, 10);

		primitive	= new AnonShorten (len);

	} else if (name.compare ("AnonBytewiseHashSha1")	== 0) {
		primitive = new AnonBytewiseHashSha1 ();
	} else if (name.compare ("AnonHashHmacSha1")		== 0) {

		Triggerconf::ATTRIBUTE_MAP_ITERATOR it	= attrmap.find ("key");
		string								key;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonHashHmacSha1: missing key attribute" << std::endl;
			exit (0);
		}

		key	= it->second;

		primitive = new AnonHashHmacSha1 (key);

	} else if (name.compare ("AnonBytewiseHashHmacSha1")== 0) {

		Triggerconf::ATTRIBUTE_MAP_ITERATOR it	= attrmap.find ("key");
		string								key;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonBytewiseHashHmacSha1: missing key attribute" << std::endl;
			exit (0);
		}

		key	= it->second;

		primitive = new AnonBytewiseHashHmacSha1 (key);

	} else if (name.compare ("AnonShuffle") == 0) {
		primitive = new AnonShuffle ();
	} else if (name.compare ("AnonWhitenoise") == 0) {

		Triggerconf::ATTRIBUTE_MAP_ITERATOR it	= attrmap.find ("strength");
		string								sstrength;
		unsigned int						istrength;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonWhitenoise: missing strength attribute" << std::endl;
			exit (0);
		}

		sstrength	= it->second;
		istrength	= strtoul (sstrength.c_str (), NULL, 10);
		
		primitive = new AnonWhitenoise (istrength);

	} else if (name.compare ("AnonCryptoPan") == 0) {

		Triggerconf::ATTRIBUTE_MAP_ITERATOR it	= attrmap.find ("key");
		string								skey;

		if (it == attrmap.end ()) {
			cerr << "incomplete configuration for AnonCryptoPan: missing key attribute" << std::endl;
			exit (0);
		}

		primitive = new AnonCryptoPan (skey);

	} else if (name.compare ("AnonBroadcastHandler") == 0) {

		primitive = new AnonBroadcastHandler ();

	} else {

		cerr << "unknown anonprimitive " << name << std::endl;
		primitive = NULL;

	}

	primitive->setNext (remainingchain);
	return primitive;
}

