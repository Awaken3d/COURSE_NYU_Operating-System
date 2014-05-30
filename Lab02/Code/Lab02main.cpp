/*
 * File name     : Lab02main.cpp 
 * By            : Jingxin Zhu
 * Last Modified : 05/30/2014
 * ------------------------------------------------------------------------------------
 * 1. Compile: in terminal, type:
 *       
 *       make
 *
 * 2. Result: 'sched.out' requires at least two parameters, one for input file, 
 *    and another for random number file. Use '-v' to print detailed trace information
 *    and use '-s' to choose different algorithms. For example, (omit $ when in terminal)
 *
 * 		 $./sched.out input1 rfile       // FCFS mode and no trace info by default.
 * 		 $./sched.out -sL input1 rfile   // select LCFS mode.
 * 		 $./sched.out -sR2 input1 rfile  // select RR mode with quantum 2.
 * 		 $./sched.out -v input1 rfile    // verbose mode, display trace info
 *
 * This will generate the result we want on the console.
 *
 * 3. Batch Test: aftering coping ./sched.out into BatchTest directory, type in terminal
 *     
 *       $ cd BatchTest
 *       $ sh ./runit.sh
 *       $ sh ./diffit.sh . ../ValidationData
 * 	 
 * 	  This will take input files in BatchTest directory, generate results and compare with
 * 	  expected results in ValidationData directory.
 *   
 * --------------------------------------------------------------------------------------
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits.h>
#include <stdlib.h> /* for atoi */
#include <unistd.h> /* for Getopt, GNU command style */

#include "DES.h"
#include "Event.h"
#include "Scheduler.h"

using namespace std;

/* Member Functions */
bool parseCommand(int argc, char* argv[], bool& v, string& s);
void readFile(DES* des, string filename, double& cpuTime);
void readRandomFile(string randfile);
int createBurst(int& ofs, int type, Event& e);
int randGen(int& ofs, int bound);
Scheduler* initScheduler(string& algo, string s, int& q);

/* Private Instances */
vector<int> randVals; // vector of random numbers 
int scale;            // size of randVals

/*      Main Function    */
int main (int argc, char* argv[]) {

    DES* des = new DES();   // initialize Discrete Event Simulator (DES)
    Scheduler* scheduler;   // initialize process scheduler
    bool vcommand = false;  // by default, do not print detailed trace information
    string scommand = "F";  // by default, select FCFS as schduling algorithm

    string algo;            // algorithm that process scheduler will implement
    int quantum = INT_MAX;  // quantum required by Round Robin, INT_MAX by default
    double cpuTime = 0;     // total time CPU running, 0 by default

    // 1. Parse console commands from user.
    if ( !parseCommand(argc, argv, vcommand, scommand) ) {
        exit(0);
    }

    string filename =  string(argv[argc-2]); // file containing processes' info
    string randfile =  string(argv[argc-1]); // file containing random numbers

    scheduler = initScheduler(algo, scommand, quantum); 

    // 2. Read in input file and random number file.
    readFile(des, filename, cpuTime);
    readRandomFile(randfile);

    // 3. DES: Simulate DES until all processes are finished.
    int type;        // event's type
    int curTime;     // current time
    int ioStart;     // beginning time for current process enters I/O
    int ioTime = 0;  // total I/O time that at least one process is in I/O 
    int ioCt   = 0;  // counter for processes that are in I/O status
    int ofs    = 0;  // offset, indicating index of random number to choose in randVals
    int burst;       // burst time for CPU or I/O
    int cpuStatus = 0; // CPU status:0 for CPU free, 1 for busy
    int ioStatus  = 0; // I/O status: 0 for no process is in I/O state, 1 otherwise

    // Terminate whole simulation when all processes are finished,
    // namely no process is in DES.
    while ( !des->isEmpty() ) {		

        /* PART 1--- DES PART: get event and process event--- */

        // 1. Get new event 
        Event event = des->getEvent();

        // 2. Process all events arriving at the same time stamp
        // 	  before invoking the scheduler  
        while (true) {
            /* 2.1 Process event based on event's type
             * 	   type 1: RUNNG->BLOCK ==> create IO burst ==> BLOCK->READY
             *     type 2: READY->READY ==> READY->RUNNG
             *     type 4: BLOCK->READY ==> READY->RUNNG
             *     type 5: RUNNG->READY ==> READY->RUNNG
             *         PS: no type 3 events (READY->RUNNG) in DES, 
             *             they are in scheduler's "READY" queue
             */
            type = event.getType();

            curTime = event.ts;

            des->deleteEvent();

            // For type 2 events, add a new event  
            // into scheduler's "READY" queue.
            if (type == 2) { // READY->READY ==> READY->RUNNG
                if (vcommand == true) cout << event << endl;
                des->createEvent(type,event);
                scheduler->addReady(event);
            }

            if (type == 5) { // RUNNG->READY ==> READY->RUNNG
                cpuStatus = 0;  // set CPU to 'Free'
                if (vcommand == true) cout << event << endl;
                des->createEvent(type,event);
                scheduler->addReady(event);
            }

            if (type == 4) { // BLOCK -> READY ==> READY -> RUNNG
                ioCt--;  // one process leaves I/O state	
                if (ioCt == 0){ // if no process is in I/O
                    ioStatus = 0;
                    ioTime += curTime - ioStart;
                }
                if (vcommand == true) cout << event << endl;
                des->createEvent(type,event);
                scheduler->addReady(event);
            }

            if(type == 1){ // RUNNG -> BLOCK => BLOCK -> READY
                cpuStatus = 0;
                // if this process is finished, add it into complete list;
                // otherwise, it will enter I/O
                if ( event.isFinish() ) {
                    if (vcommand == true) event.printDone();
                    des->addCompleteEvent(event);
                } else { 
                    if (ioStatus==0) {
                        ioStatus = 1;
                        ioStart = curTime;
                    }
                    ioCt++;
                    // --- Old event ---
                    // 1) create I/O burst
                    burst = createBurst(ofs,type,event);
                    // 2) modify event's I/O burst	
                    event.setIOburst(burst);
                    // 3) print event
                    if (vcommand == true) cout << event << endl;
                    // --- New event --- BLOCK -> READY
                    // 4) modity time stamp
                    event.recordTime();
                    event.ts += burst;
                    // 5) create event
                    des->createEvent(type,event);
                    // 6) add into DES
                    des->setEvent(event);
                }
            }	

            /* Check whether we need delay scheduler 
             * delay scheduler if events have the same arriving time.
             */
            if (des->isEmpty()) {
                break;
            } else {
                event =des->getEvent();
                if ( event.ts != curTime){ 	
                    break;  // Get into Scheduler Part
                }
                if ( event.getType() != 1 && type !=4){
                    if (vcommand == true) cout << "   delay scheduler" << endl;
                }
            }
        } // end of while, go to scheduler part

        /* PART 2. ---SCHEDULER PART: ---
         * schedule event when CPU is unoccupied && "READY" queue is not empty
         */
        if ( (cpuStatus==0) && (!scheduler->isEmpty()) ) { // Scheduler is ready!
            event = scheduler->getReady();  // READY -> RUNNG
            type = event.getType();
            burst = event.getCPUburst();
            cpuStatus = 1;  
            if (burst==0) {  // if this is from BLOCK->READY, create CPU burst
                // 1) create CPU burst
                burst = createBurst(ofs,type,event);
                // 2) modity CPU burst time
                event.setCPUburst(burst);
            }
            event.recordTime();
            event.ts = curTime;
            if (vcommand == true) cout << event << endl;

            // --- New event ---
            // 1) modify remaining time
            if (quantum < burst) { // PREEMPT!!! 
                burst = burst - quantum;
                event.setCPUburst(burst);
                event.setRemain(type, quantum);
                // 2) modity time stamp 
                event.recordTime();
                event.ts += quantum;
                // 3) create new event
                event.setType(5);
                des->setEvent(event);
                curTime = event.ts;
            } else {  // RUNNG -> BLOCK
                event.setRemain(type, burst);
                event.recordTime();
                event.ts += burst;

                des->createEvent(type,event);
                event.setCPUburst(0);
                des->setEvent(event);
            }
        }

        // Terminate program when no events left
        if ( scheduler->isEmpty() && des->isEmpty()) {
            break;
        }
    }

    // 4. Output expected results.
    cout << algo;
    if (algo=="RR"){
        cout << " " <<  quantum;
    }
    cout << endl;

    des->printSummary(curTime, cpuTime, ioTime);
    
    return 0;
}

/*   Private Functions   */
void readFile(DES* des, string filename, double& cpuTime) {
    int pid = 0;
    int AT,TC,CB,IB;
    ifstream input;
    input.open(filename.c_str());
    string line;
    while (getline(input, line)){
        istringstream tokens(line);
        tokens >> AT >> TC >> CB >> IB;
        Event e(pid, 2, AT, TC, CB, IB);
        cpuTime += TC;
        des->setEvent(e);
        pid++;	
    }
    input.close();
}

void readRandomFile(string randfile){
    int number;
    string line;
    ifstream input;
    input.open(randfile.c_str());
    getline(input, line);
    istringstream tokens(line);
    tokens >> scale;
    while(getline(input, line)){
        istringstream tokens(line);
        tokens >> number;
        randVals.push_back(number);
    }
    input.close();
}

int createBurst(int& ofs, int type, Event& e) {
    int burst;
    if (type == 3) { // create CPU burst
        burst = randGen(ofs, e.CB);	
        e.setCPUburst(burst);
        burst = e.getCPUburst();
    }
    if (type == 1) { // create IO burst
        burst = randGen(ofs, e.IB);
    }
    return burst;
}

int randGen(int& ofs, int bound) {
    if (ofs >= scale) {
        ofs = 0;
    }
    int result = 1 + randVals[ofs] % bound;
    ofs++;
    return result;
}

bool parseCommand(int argc, char* argv[], bool& v, string& s) {
    int c;
    opterr = 0;
    while ( (c = getopt(argc, argv, "vs:")) != -1 )
        switch (c) {
            case 'v':
                v = true;
                break;
            case 's':
                s = string(optarg);
                break;
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument. \n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option '-%c'.\n", optopt);
                return false;
            default:
                abort ();
        }
    return true;
}

Scheduler* initScheduler(string& algo, string s, int& q) {
    if ( s == "F") {
        algo = "FCFS";
        return new SchedulerFCFS();
    } else if (s == "L") {
        algo = "LCFS";
        return new SchedulerLCFS();
    } else if (s == "S") {
        algo = "SJF";
        return new SchedulerSJF();
    } else {
        algo = "RR";
        q = atoi(s.substr(1).c_str());
        return new SchedulerRR(q);
    }
}
