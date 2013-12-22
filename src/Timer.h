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

#ifndef __TIMER_H
#define __TIMER_H

#include "stdlib.h"
#include <memory.h>
#include <iostream>

#ifdef WIN32
	#define _WIN32_WINNT 0x0500
	#include "Windows.h"
#elif defined LINUX
	#include <sys/time.h>	
	#include <pthread.h>
#endif

using std::cerr;

//
// Timer class for Windows and Linux.
// Resolution is in seconds, as this sufficies our use.
// The Windows part uses CreateTimerQueueTimer Win32 API,
// the linux part uses a pthread and sleeps. The first implementation
// for Linux used SIGALARM signals. But inside the signal handler 
// quite about no library function calls can be made safely. After
// a while it deadlocked. The Windows part could also be done using a
// pthread but this method is ugly anyway because the timer can't stop
// when we call the stop function but rather has to wait until the next
// timeout elapsed. So at least the Windows part is clean ...
//

class Timer {
public:
	
	typedef void			(*TIMER_CALLBACK)		(void* userdata);

							Timer					();
							~Timer					();

	bool					start					(unsigned int _secs, TIMER_CALLBACK _callfunc, void* _userdata);
	bool					stop					();

private:

	void*					userdata;	
	TIMER_CALLBACK			callfunc;
	unsigned int			secs;
	volatile bool			running;

#ifdef WIN32
	static VOID CALLBACK	timersyscall			(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	HANDLE					timerhandle;
#elif defined LINUX
	static void				timersyscall			(void* parameter);	
	pthread_t				waitthreadObj;
	static void*			waitthreadFunc			(void* obj);
#endif

};

#endif // __TIMER_H
