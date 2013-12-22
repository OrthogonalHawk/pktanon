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

#include "StatisticEngine.h"

StatisticEngine::StatisticEngine(void) 
: lastmark (time (NULL), 0, 0)
{
	starttime	= 0;
	endtime		= 0;

	packets		= 0;
	skipped		= 0;
	bytes		= 0;

	pthread_mutex_init (&measuremutex, NULL);
}

StatisticEngine::~StatisticEngine(void)
{
}

void StatisticEngine::openMeasureFile (const string& file)
{
	measurefilename = Utils::trim (file);
	if (measurefilename.length () <= 0) return;

	outfile.open	(measurefilename.c_str(), ios_base::out | ios_base::trunc);
	dumpString		("# === measure marks === \n");
	dumpString		("# \t bytes -\t packets -\t duration -\t bytespersecond -\t packetspersecond -\t secondsperpacket -\t timedifflastmark \n");
}

void StatisticEngine::closeMeasureFile ()
{
	if (outfile.is_open ()) 
		outfile.close ();
}

void StatisticEngine::start ()
{
	starttime = time (NULL);
	bool ret = timer.start (STATISTIC_MEASURE_INTERVAL, &timerhandler, this);
	
	if (ret == false) {
		cerr << "starting timer failed" << std::endl;
		exit (0);
	}
}

void StatisticEngine::timerhandler (void* userdata)
{
	assert (userdata != NULL);
	StatisticEngine* engine = (StatisticEngine*)userdata;

	//
	// create a measure mark, dump it to the file
	// and set the last measuremark to this new one
	//

	pthread_mutex_lock (&engine->measuremutex);

	MEASURE_MARK newmark (time (NULL), engine->packets, engine->bytes);
	engine->dumpMeasureMark (&newmark, &engine->lastmark);
	engine->lastmark = newmark;

	pthread_mutex_unlock (&engine->measuremutex);
}

void StatisticEngine::dumpMeasureMark (PMEASURE_MARK current, PMEASURE_MARK last)
{
	unsigned int duration = (unsigned int) difftime (current->time, starttime);
	unsigned long long packetspan = current->packets - last->packets;

	//
	// make sure that we do not divide through zero ...
	//

	if (packetspan	== 0) ++packetspan;

	//
	// print out the column values
	//
	
	ostringstream out;

	out << " \t " << current->bytes 
		<< " \t " << current->packets 
		<< " \t " << duration
		<< " \t " << (current->bytes - last->bytes)	/ STATISTIC_MEASURE_INTERVAL
		<< " \t " << (current->packets - last->packets)	/ STATISTIC_MEASURE_INTERVAL
		<< " \t " << (double) STATISTIC_MEASURE_INTERVAL / (double) packetspan
		<< " \t " << STATISTIC_MEASURE_INTERVAL
		<< std::endl;

	dumpString (out.str ());
}

void StatisticEngine::stop ()
{
	timer.stop ();
	endtime	= time (NULL);
}

string StatisticEngine::getStatistics ()
{
	//
	// print out speed and misc statistics
	//

	double mb		= (double)((double)bytes / (1000.0 * 1000.0));
	double duration = difftime (endtime, starttime); 
	double kbytes	= (double)((double) bytes / 1000.0) / duration;
	double kbits	= kbytes * 8.0;
	double mbytes	= kbytes / 1000.0;
	double mbits	= mbytes * 8.0;
	double gbytes	= mbytes / 1000.0;
	double gbits	= gbytes * 8.0;

	ostringstream out;

	out		<< "# === statistic summary === " 	<< std::endl
			<< "# [ "			<< " "			
			<< packets			<< " packets"		<< " : "		
			<< skipped			<< " skipped"		<< " : " 		
			<< mb				<< " mbyte data"	<< " : " 		
			<< duration			<< " seconds"		<< " : " 		
			<< kbits			<< " kbit/s"		<< " : " 		
			<< kbytes			<< " kbyte/s"		<< " : " 		
			<< mbits			<< " mbit/s"		<< " : " 		
			<< mbytes			<< " mbyte/s"		<< " : " 		
			<< gbits			<< " gbit/s"		<< " : " 		
			<< gbytes			<< " gbyte/s"		<< " "
			<< " ] "			<< std::endl;				

	//
	// print out protocol statistics
	//

	PROTOCOL_DISTRIBUTION_MAP::iterator protocolsbegin	= protocolmap.begin ();
	PROTOCOL_DISTRIBUTION_MAP::iterator protocolsend	= protocolmap.end	();

	out		<< "# === protocol statistics === " << std::endl	
			<< "# [";
	
	for ( ; protocolsbegin != protocolsend; protocolsbegin++) {
		if (protocolsbegin != protocolmap.begin()) out << ", ";
		out << Packet::getProtocolName ((*protocolsbegin).first) << ": " << (*protocolsbegin).second;
	}

	out << " ] " << std::endl;

	//
	// done, return stats string
	//

	return out.str ();
}

string StatisticEngine::toString ()
{
	string stats = getStatistics ();
	dumpString (stats);

	return stats;
}

void StatisticEngine::dumpString (string str)
{
	if (! outfile.is_open ()) return;
	outfile.write (str.c_str(), (int) str.length());
}

void StatisticEngine::addPackets (int _packets)
{
	packets += _packets;
}

void StatisticEngine::addBytes (int _bytes)
{
	bytes += _bytes;
}

void StatisticEngine::addSkipped (int _skipped)
{
	skipped += _skipped;
}

void StatisticEngine::resetBytes ()
{
	bytes = 0;
}

void StatisticEngine::addProtocol (Packet::PROTOCOL proto)
{
	PROTOCOL_DISTRIBUTION_MAP::iterator	item = protocolmap.find (proto);

	if (item == protocolmap.end ()) 
		protocolmap.insert (PROTOCOL_DISTRIBUTION_PAIR (proto, 1));
	else
		item->second++;
}

void StatisticEngine::createPlot ()
{
	//
	// get date/time string for plot title
	//

	struct tm*	timeval;
    time_t      clockval;
	string		title;

    time	(&clockval);
    timeval	= localtime (&clockval);
	title	= measurefilename + " - recorded " + Utils::trim (asctime (timeval));

	//
	// create temporary gnuplot command file
	//

	string::size_type pointpos   = measurefilename.find_last_of (".");
	string::size_type slashpos   = measurefilename.find_last_of (Utils::DIRECTORY_DELIMITER);
	string measurefilename_noext = measurefilename;

	if (pointpos != string::npos && pointpos > slashpos)
		measurefilename_noext = measurefilename.substr (0, pointpos);

	string gnuplotfile	= measurefilename_noext + ".plt";
	string outfile		= measurefilename_noext + ".eps";

	string text;
	text += "set title '" + title + "'\n";
	text += "set xlabel 'Duration [s]'\n";
	text += "set ylabel 'Speed [MBit/s]'\n";
	text += "set terminal postscript\n";		
	text += "set output '" + outfile + "'\n";		
	text += "plot '" + measurefilename + "' using 3:(($4*8)/(1000*1000)) with lines title '" + measurefilename + "'";

	ofstream tempfile	(gnuplotfile.c_str(), ios_base::out | ios_base::trunc);
	tempfile.write		(text.c_str(), (int)text.length());
	tempfile.close		();

	//
	// execute gnuplot, close the measure-mark file before
	// in case it if still open
	//

	closeMeasureFile ();
	cerr << "plotting data to " << outfile << std::endl;

#ifdef WIN32
	
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory (&si, sizeof (si));
    si.cb = sizeof (si);
    ZeroMemory (&pi, sizeof (pi));

	string commandline = "wgnuplot.exe" + string (" \"") + gnuplotfile + string ("\"");
	int ret = CreateProcess (	NULL,
								(char*) commandline.c_str(),
								NULL,
								NULL,
								FALSE,
								0,
								NULL,
								NULL,
								&si,
								&pi	);

	//
	// if the process has been created, wait for it to finish
	// otherwise we will delete the tmpfile before gnuplot is done
	//

	if (ret == 0)
		cerr << "executing gnuplot failed with error code " << GetLastError() << std::endl;			
	else
		WaitForSingleObject (pi.hProcess, INFINITE);

#else
	
	pid_t pid = fork ();
	
	if (pid == 0) { // new process child
		
		char* args [] = {"gnuplot", (char*)gnuplotfile.c_str(), 0};
		int ret = execv ("/usr/bin/gnuplot", args);
		if (ret == -1)
			cerr << "error executing gnuplot: " << errno << std::endl;

	} else { // parent, main process
	
		int	status;
		waitpid (pid, &status, WUNTRACED
#ifdef WCONTINUED       /* Not all implementations support this */
        | WCONTINUED
#endif
		);

	}

#endif

	//
	// do not remove the gnuplot command file
	// maybe it is still needed for adaption
	// To remove it anyway uncomment the next line
	//
	// remove (gnuplotfile.c_str());
}
