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

#include "Timer.h"
#include "unistd.h"

Timer::Timer ()
:	userdata	(NULL),
	callfunc	(NULL),
	secs		(0),
	running		(false)
{
}

Timer::~Timer ()
{
}

bool Timer::start (unsigned int _secs, TIMER_CALLBACK _callfunc, void* _userdata)
{
	userdata	= _userdata;
	callfunc	= _callfunc;
	secs		= _secs;
	running		= true;

#ifdef LINUX

	int ret = pthread_create (&waitthreadObj, NULL, waitthreadFunc, this);
	return (ret == 0);

#elif defined WIN32

	BOOL ret = CreateTimerQueueTimer (	&timerhandle, 
										NULL, 
										&Timer::timersyscall, 
										this, 
										secs*1000, 
										secs*1000, 
										0	);
	return (ret == TRUE);

#endif
}

bool Timer::stop ()
{
	running = false;

#ifdef LINUX
	
	pthread_join (waitthreadObj, NULL);

#elif defined WIN32
	
	return (DeleteTimerQueueTimer (NULL, timerhandle, NULL) == TRUE);

#endif
}

#ifdef LINUX
void* Timer::waitthreadFunc (void* obj)
{
	Timer* timer = (Timer*)obj;

	while (timer->running) {

		timer->timersyscall (obj);
		sleep (1);

	} // while (timer->running)

	return NULL;
}
#endif // LINUX

#ifdef WIN32
	void Timer::timersyscall (PVOID parameter, BOOLEAN TimerOrWaitFired)
#elif defined LINUX
	void Timer::timersyscall (void* parameter)
#endif
{
	Timer* timer = (Timer*)parameter;
	(timer->callfunc) (timer->userdata);
}
