/* File: 
 * ---------------
 * By   : Jingxin Zhu
 * Date : 
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdio>

#include "Operation.h"
#include "IOsystem.h"
#include "SchedulerBase.h"
#include "SchedulerFIFO.h"
#include "SchedulerSCAN.h"
#include "SchedulerSSTF.h"
#include "SchedulerCSCAN.h"
#include "SchedulerFSCAN.h"

using namespace std;

// private functions
void readFile(string filename);
void initSystem(string algo);

// private instances
Operation oper;
vector<Operation> opVector;
IOsystem* iosys; 
SchedulerBase* scheduler;
int iosize;

int main(int argc, char *argv[]) {
	string algo = "i";
	vector<string> paras;
	for (int i=0; i< argc; i++) {
		paras.push_back(string(argv[i]));	
	}
	for (int i=0; i<argc; i++) {
		if (paras[i].find("-s") != string::npos){
			algo = paras[i].substr(2);
		}
	}
	vector<string>::iterator it;
	

	//string filename = "in0";
	string filename = paras.back();
	/* I. Initialization */
	iosys = new IOsystem();
	initSystem(algo);

	int curTrack = 0; // current track;
	int curTime = 1;  // current time
	int type;
	int disk = 0; // status of disk, 0 for free, 1 for busy
	int dur = 0;
	
	int totalMove = 0;
	int totalTurnaround = 0;
	int totalWaittime = 0;
	/* II. Read in file */
	readFile(filename);
	int i = 0;
	int trace = 0;

	/* III. Simulation */
	if (trace == 1){
		printf("TRACE\n");
	}
	// While system has requests, contine
	while ( !iosys->isEmpty() ) {	
	//while (i < 4) {
		// 1. Get event from I/O subsystem
		oper = iosys->getEvent();	
		curTime = oper.ft;

		// 1.0 set current time and delete event from system
		type = oper.type;
		iosys->deleteEvent();
		// 1.1 if 'add' event,
		//     set oper into scheduler
		if (type == 1) {
			if (trace == 1) printf("%d: %5d add %d\n", curTime, oper.op, oper.track);
			scheduler->setOperation(curTime);					
		}
		// 1.2 if 'finish' event, 
		// 	   set disk to be free
		if (type == 2) {
			if (trace == 1) printf("%d: %5d finish %d\n", oper.ft, oper.op, oper.ft-oper.at);
			totalTurnaround +=  oper.ft - oper.at;
			totalWaittime += oper.st - oper.at;
			iosys->setFinish();	
			disk = 0;	
		}

		// 3. Schedule oper if 
		//    Disk is free and scheduler is not empty
		if ( (disk==0) && (!scheduler->isEmpty()) ) {
			  // 3.1 scheduler issues operation 
			  //     based on schedule algorithm
			oper = scheduler->getOperation();
			if (trace == 1) printf("%d: %5d issue %d %d\n", 
				curTime, oper.op, oper.track, curTrack);
	   		  // 3.2 set 'finish' event into system
			dur = curTrack - oper.track;
			if (dur > 0){
				totalMove += dur;
			} else {
				totalMove += -dur;
			}
			curTrack = oper.track;
			oper.st = curTime;
			iosys->setEvent(dur);
			  // 3.3 set Disk to be busy 
			disk = 1;
			  // 3.4 delete operation from scheduler's queue
			scheduler->deleteOperation();
			
		}

		// 4. Stop if both system and scheduler are empty
		//printf("io size: %d sched size: %d \n", iosys->getSize(), scheduler->getSize());
		if (iosys->isEmpty() && scheduler->isEmpty()) break;
		i++;	
	}
	/* IV. print summary info */
	//iosys->printFinish();
	iosys->printSum(curTime, totalMove);
	return 0;
}

/* Private Functions */
void readFile(string filename) {
	int ct = 0;
	int time, track;
	ifstream input;
	input.open(filename.c_str());
	string line;
	while (getline(input, line)) {
		if (line[0] == '#') continue;
		istringstream tokens(line);
		tokens >> time >> track;
		oper.op = ct;
		oper.at = time;
		oper.st = time;
		oper.ft = time;
		oper.type = 1; // "add" event
		oper.track = track;
		opVector.push_back(oper);	
		iosys->addEvent(oper);	
		ct++;
	}
	iosize = ct;
}

void initSystem(string algo) {
	if (algo == "i") scheduler = new SchedulerFIFO(); 
	if (algo == "j") scheduler = new SchedulerSSTF(); 
	if (algo == "s") scheduler = new SchedulerSCAN(); 
	if (algo == "c") scheduler = new SchedulerCSCAN(); 
	if (algo == "f") scheduler = new SchedulerFSCAN(); 
}
