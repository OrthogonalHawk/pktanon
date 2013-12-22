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

#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#include "triggerconf.h"
#include "anonprimitives/AnonFactory.h"
#include "anonprimitives/AnonPrimitive.h"
#include <string>
#include <iostream>

using std::string;
using std::cerr;

class Configuration 
{
public:
	
	static void					init					(const string& file);
	static Configuration*		instance				();

	string						getFlowSource			();
	string						getFlowDest				();
	bool						getUseMeasure			();
	string						getMeasureFile			();
	bool						getCreatePlot			();
	bool						getReCalcChecksums		();
	bool						getSetBadChecksumsToBad	();
	bool						getPrintPackets			();
	bool						getSingleStepping		();
	unsigned long				getReadBlockSize		();
	unsigned long				getWriteBlockSize		();

	void						bindAnonPrimitives		();
	void						releaseAnonPrimitivs		();
	bool						configOk				();

	static const string			PATH_RUN_FLOW_SOURCE;
	static const string			PATH_RUN_FLOW_DEST;
	static const string			PATH_RUN_MEASURE_ON;
	static const string			PATH_RUN_MEASURE_FILE;
	static const string			PATH_RUN_MEASURE_CREATEPLOT;
	static const string			PATH_RUN_PRINT_PACKETS;
	static const string			PATH_RUN_SINGLE_STEPPING;
	static const string			PATH_RUN_BLOCK_SIZE_READ;	
	static const string			PATH_RUN_BLOCK_SIZE_WRITE;	
	static const string			PATH_CHKSUM_RECALC;
	static const string			PATH_CHKSUM_BAD_TO_BAD;

	static const string			ANON_MODULE;

	static const string			ANON_SUBMODULE_ETHERNET;
	static const string			ANON_SUBMODULE_VLAN;
	static const string			ANON_SUBMODULE_ARP;
	static const string			ANON_SUBMODULE_IP;
	static const string			ANON_SUBMODULE_ICMP;
	static const string			ANON_SUBMODULE_TCP;
	static const string			ANON_SUBMODULE_UDP;
	static const string			ANON_SUBMODULE_PAYLOAD;
	static const string			ANON_SUBMODULE_IPV6;

	static const string			ANON_ATTR_PRIMITIVE;

private:
								Configuration			(const string& filename);
								~Configuration			();

	class Guard {
		public: ~Guard() {
			if (Configuration::object != NULL){
				delete Configuration::object;
				Configuration::object = NULL;
			}
		}
	};

	friend class				Guard;

	bool						checkAnonObj			(AnonPrimitive* anon, string name);

	static Configuration*		object;
	static string				filename;

	Triggerconf					tconf;
	void						exitOnBad				();

};

#endif // __CONFIGURATION_H

