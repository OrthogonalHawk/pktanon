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

#ifndef __STATISTIC_ENGINE
#define __STATISTIC_ENGINE

#include <sstream>
#include <fstream>
#include <ctime>
#include <string>
#include <cassert>
#include <iostream>
#include <map>
#include <ios>
#include <ctime>
#include "Timer.h"
#include "Utils.h"
#include "packets/Packet.h"

#ifdef WIN32
	#include <process.h>
	#include "../lib/pthreadwin32/pthread.h"
#else
	#include <sys/types.h>
	#include <sys/wait.h> 
	#include <pthread.h>
	#include <errno.h>
#endif

using std::pair;
using std::map;
using std::string;
using std::cerr;
using std::ostringstream;
using std::ofstream;
using std::ios_base;

#define STATISTIC_MEASURE_INTERVAL	1

class StatisticEngine
{
public:
							StatisticEngine		(void);
							~StatisticEngine	(void);

	void					openMeasureFile		(const string& file);
	void					closeMeasureFile	();
	void					start				();
	void					stop				();
	string					toString			();
	void					createPlot			();

	void					addPackets			(int _packets = 1);
	void					addBytes			(int _bytes);
	void					addSkipped			(int _skipped = 1);
	void					addProtocol			(Packet::PROTOCOL proto);

	void					resetBytes			();

private:

	typedef struct _MEASURE_MARK {
		time_t				time;				
		unsigned long long	packets;
		unsigned long long	bytes;

		_MEASURE_MARK (time_t _time, unsigned long long _packets, unsigned long long _bytes) {
			time	= _time;
			packets	= _packets;
			bytes	= _bytes;
		}
	} MEASURE_MARK, *PMEASURE_MARK;

	Timer					timer;
	static void				timerhandler		(void* userdata);

	time_t					starttime;
	time_t					endtime;

	unsigned long long		packets;
	unsigned long long		skipped;
	unsigned long long		bytes;

	ofstream				outfile;
	string					measurefilename;
	pthread_mutex_t			measuremutex;
	MEASURE_MARK			lastmark;
	void					dumpMeasureMark		(PMEASURE_MARK current, PMEASURE_MARK last);
	void					dumpString			(string str);
	string					getStatistics		();
		
	typedef pair<Packet::PROTOCOL, unsigned long long>	PROTOCOL_DISTRIBUTION_PAIR;
	typedef map <Packet::PROTOCOL, unsigned long long>	PROTOCOL_DISTRIBUTION_MAP;
	PROTOCOL_DISTRIBUTION_MAP							protocolmap;

};

#endif // __STATISTIC_ENGINE
