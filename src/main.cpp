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

//
// use this block to enable memory profiling under Win32
// the boost::pool can not be used then and will be 
// automaticall undefined
//
//#if (defined(_DEBUG) | defined (DEBUG)) & defined(WIN32)
//	#define _CRTDBG_MAP_ALLOC
//	#include <stdlib.h>
//	#include <crtdbg.h>
//#endif
//

//
// to enable fine grained measurement, several
// execution stages are defined. Using define
// flags that can also be invoked from the makefile
// the code only works his way up to the defined stage. This way
// we can measure the runtime of the different operations/stages.
// The stages are listed here in the specified run order
// subsequent stages include the ones before. The 
// OPERATION_STAGE_OUTPUT level is equal to setting no special
// defines because it does all the processing. 
//
// Make sure that only _one_ stage is defined!
//
//#define OPERATION_STAGE_READ			1
//#define OPERATION_STAGE_PARSE			1
//#define OPERATION_STAGE_TRANSFORM		1
//#define OPERATION_STAGE_OUTPUT		1
//

//
// use this define to run the anonymization
// primitive benchmarking rather then the normale
// pktanon
// TODO:	the whole anon primitive benchmarking
//			should be offloaded into a completly
//			different program ... this is hacky
//
//#define ANON_BENCHMARK 1
//

//*****************************************************

#include <csignal>
#include <string>
#include <iostream>
#include <cassert>

#include "PcapFile.h"
#include "Transformer.h"
#include "StatisticEngine.h"
#include "Configuration.h"

#ifdef ANON_BENCHMARK
	#include "benchmark/AnonPrimitiveBenchmark.h"
#endif

#ifdef WIN32
	#include "../lib/pthreadwin32/pthread.h"
#else
	#include <pthread.h>
#endif

using std::cerr;
using std::cin;
using std::string;

typedef struct _PSEUDO_READER_ITEMS {
	PcapFile*		file;
	volatile bool	running;
} PSEUDO_READER_ITEMS, *PPSEUDO_READER_ITEMS;

volatile bool exitcode = false;

void sighandler(int sig)
{
	//
	// we need this SIGINT handler such that we can
	// close gracefully and output the stats normally
	//

	exitcode = true;
}

void* pseudoReaderThread (void* data)
{
	//
	// we need this pseudo reader to clean stdin when capturing
	// in online mode with tcpdump. This prevents massive packet
	// loss at the beginning when we are initializing
	// we do miss the packets at the beginning (we even want to)
	// but when we get going we have a heavily reduced packet loss then
	// because the stdin buffer is empty when we really start
	//

	PPSEUDO_READER_ITEMS items = (PPSEUDO_READER_ITEMS) data;

	while (items->running) {
		items->file->readFrame ();
	}

	return NULL;
}

void assertExit (bool cond, string msg)
{
	if (cond == false) {
		cerr << msg << std::endl;
		exit (0);
	}
}

int main (int argc, char* argv[]) 
{

#if defined (WIN32) & (defined(DEBUG) | defined(_DEBUG))
	_CrtSetDbgFlag (	_CRTDBG_ALLOC_MEM_DF		| 
						_CRTDBG_LEAK_CHECK_DF		|
						_CRTDBG_CHECK_ALWAYS_DF		);
#endif

	cerr	<< "------------------" << std::endl
			<< "pktanon v" << VERSION << "  --- profile-based traffic anonymization" << std::endl
			<< "------------------" << std::endl;


#ifdef ANON_BENCHMARK

	AnonPrimitiveBenchmark bench;
	bench.run ();

	for (unsigned int i=0; i<bench.getResultCount(); i++) {
		AnonPrimitiveBenchmark::ANON_RESULT_PAIR res = bench.getResult (i);
		
		FILE* handle = fopen (("benchmark_" + res.first + ".txt").c_str (), "w");
		fprintf (handle, "%s", res.second.c_str ());
		fclose	(handle);
	}

	return 0;

#endif // ANON_BENCHMARK

	//
	// get parameters
	//
	
	string configfile = "";

#if defined (WIN32) & (defined(DEBUG) | defined(_DEBUG))
	// <debug> debugging with Visual Studio

	configfile = "C:\\_Chris\\Temp\\settings_identity.xml";
	
	// </debug>
#else
	// <release>
	
	if (argc != 2) {
		cerr << "Invalid parameters" << std::endl;	
		cerr << "Usage: pktanon settings.xml" << std::endl;
		return 1;
	} else {
		configfile = string(argv[1]);
	}

	// </release>
#endif

	//
	// create the main objects
	//

	Configuration::init	(configfile);
	Configuration*	config	= Configuration::instance ();
	PcapFile		infile	(config->getFlowSource (), Stream::RW_READ);
	PcapFile		outfile	(config->getFlowDest   (), Stream::RW_WRITE);
	Transformer		trans;
	bool			measure		= config->getUseMeasure		();
	bool			printpacket = config->getPrintPackets	();
//	StatisticEngine* statistic = new StatisticEngine();

	cerr 	<< "\tconfiguration file:\t "  << configfile              << std::endl
			<< "\tinput packet trace:\t "  << config->getFlowSource() << std::endl
			<< "\toutput packet trace:\t " << config->getFlowDest()   << std::endl;

	//
	// check for correct data type lengths
	// on the current platform
	//

	assertExit (sizeof (int8_t)			== 1, "invalid data type size for char");
	assertExit (sizeof (uint8_t)		== 1, "invalid data type size for unsigned char");
	assertExit (sizeof (int16_t)		== 2, "invalid data type size for short");
	assertExit (sizeof (uint16_t)		== 2, "invalid data type size for unsigned short");
	assertExit (sizeof (int32_t)		== 4, "invalid data type size for int");
	assertExit (sizeof (uint32_t)		== 4, "invalid data type size for unsigned int");
	assertExit (sizeof (int32_t)		== 4, "invalid data type size for long");
	assertExit (sizeof (uint32_t)		== 4, "invalid data type size for unsigned long");


	//
	// install Strg+c handler. This is needed that we can output
	// the statistics in case of a user requested termination
	//

	signal (SIGINT, sighandler);

	//
	// read or write the pcap header
	//

	if(infile.workHeader() == false) return -1;
	if(outfile.workHeader() == false) return -1;

	//
	// now we start a pseudo reader when we are reading from
	// stdin. Otherwise too much data comes in which is being dropped
	// and the read buffer is full from the start
	//
		
	pthread_t				pseudoThread;
	PSEUDO_READER_ITEMS		pseudoItems;
	
	if (infile.getStream ().getIoSource () == Stream::IO_CIN) {

		pseudoItems.file		= &infile;
		pseudoItems.running		= true;
		
		pthread_create (&pseudoThread, NULL, pseudoReaderThread, &pseudoItems);

	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CPU CONSUMING CONFIG STUFF _MUST_ GO HERE
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//
	// load the xml profile and bind the anon 
	// primitives to the protocol attributes
	//

	config->bindAnonPrimitives ();
	if (! config->configOk ()) {
		cerr << "invalid configuration" << std::endl;
		exit (0);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CPU CONSUMING CONFIG STUFF _MUST_ END HERE
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//
	// start the measure thread
	//

	if (measure) {
		//statistic->openMeasureFile	(config->getMeasureFile ());
		//statistic->start				();
	}

	//
	// stop the pseudo reader
	//

	if (infile.getStream().getIoSource() == Stream::IO_CIN) {
		pseudoItems.running = false;
		pthread_join (pseudoThread, NULL);
	}

	//
	// now we start processing the packets in the trace
	//

	Packet* inpkt	= NULL;
	Packet* outpkt	= NULL;
	Packet* tmppkt	= NULL;
	Frame*	frame	= NULL;

	for (int i=1; exitcode == false; i++) {

		try {

			frame = infile.readFrame ();
			if (frame == NULL) {
				if (infile.getStream().eof())
					break;
				
				//statistic->addSkipped (1);
				continue;
			}
			
#ifdef OPERATION_STAGE_READ
			statistic.addBytes   (frame->capturelength);
			statistic.addPackets (1);
			PacketFactory::instance()->freeFrame (frame);	
			continue;
#endif

			frame->payloadpacket->parsePacket		();
			frame->payloadpacket->createNextLayer	();

#ifdef OPERATION_STAGE_PARSE
			statistic.addBytes   (frame->capturelength);
			statistic.addPackets (1);
			PacketFactory::instance()->freeFrame (frame);	
			continue;
#endif

			inpkt = frame->payloadpacket;
			if (inpkt == NULL) {
				PacketFactory::instance()->freeFrame (frame);
				break;
			}

			if (measure) {
				for (tmppkt = inpkt; tmppkt != NULL; tmppkt = tmppkt->getNextPacket ()) {
					//if (tmppkt != NULL) statistic->addProtocol (tmppkt->getProtocol ());
				}
			}
	
			//statistic->addBytes (inpkt->getSize ());

			if (printpacket) {
				cerr << "------------------start original packet " << i << "------------------\n";
				cerr << frame->toString ();
				cerr << "------------------end   original packet " << i << "------------------\n";
			}

			outpkt = trans.getTransformedPacket (*inpkt);
			if (outpkt == NULL) {
				//statistic->addSkipped (1);
				PacketFactory::instance()->freeFrame (frame);
				continue;
			}
			
#ifdef OPERATION_STAGE_TRANSFORM	
			statistic.addPackets (1);
			PacketFactory::instance()->freeFrame	(frame);
			PacketFactory::instance()->freePacket	(outpkt);
			continue;
#endif

			frame->payloadpacket	= outpkt;
			outfile.writeFrame		(*frame);
			//statistic->addPackets	(1);

			if (printpacket) {
				cerr << "-----------------start anonymized packet " << i << "-----------------\n";
				cerr << frame->toString ();
				cerr << "-----------------end   anonymized packet " << i << "-----------------\n";
			}

			PacketFactory::instance()->freeFrame	(frame);
			PacketFactory::instance()->freePacket	(inpkt);

#ifdef OPERATION_STAGE_OUTPUT		
			// does not execute any special handling and continues to flow
			// normally, as this is the end stage for the normal frame processing ...
#endif

			//
			// if we are in single stepping mode
			// we print out some identifier and
			// wait here for some keyboard input
			//

			if (config->getSingleStepping ()) {
				cerr << "pktanon singlestepping mode :: waiting for keyboard input >> ";	
				getchar	();
			}


		} catch (...) {

			//statistic->addSkipped (1);

		}

	} // for (int i=0; true; i++)

	//
	// output the runtime statistics
	//

	if (measure) {

		//
		// stop the timer, write down the stats (will auto write into measurefile)
		// and then close the measure file
		//

		//statistic->stop ();
		//cerr << std::endl << statistic->toString () << std::endl;
		//statistic->closeMeasureFile ();

		//
		// create a plot using gnuplot
		//

		//if (config->getCreatePlot())
		//	statistic->createPlot ();
	}

	//
	// destroy all anon primitives
	//

	config->releaseAnonPrimitivs();
	//delete statistic;
	cerr << "anonymization done" << std::endl;
	return 0;
}
