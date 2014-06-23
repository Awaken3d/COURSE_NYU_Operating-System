/*
 * File: a.cpp
 * ------------------
 * By   : Jingxin Zhu
 * Date : 04/2014
 * Usage: 
 * 	1. to compile, 
  	      g++ -o mmu a.cpp FaultHandler.cpp HandlerFIFO.cpp HandlerSC.cpp
 	      HandlerClock.cpp HandlerRandom.cpp HandlerLRU.cpp HandlerNRU.cpp
	      HandlerAging.cpp
	   Or type 'make' to take advantage of the makefile
	2. to run, 
	   In last step, 'mmu' file will be generated. Then please type,
	   ./mmu -af -f32 in1K4 rfile
	   This will call FIFO algorithm, set size of physical frame to 32,
	   take in1K4 and rfile as input files.
	3. to test,
	   ./test2.sh will generate various output files for testing.
 * 		
 * ------------------
 */
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

#include "PTE.h"
#include "FaultHandler.h"
#include "HandlerFIFO.h"
#include "HandlerSC.h"
#include "HandlerClock.h"
#include "HandlerRandom.h"
#include "HandlerNRU.h"
#include "HandlerLRU.h"
#include "HandlerAging.h"

using namespace std;

struct INSTRUCTION {
	int type;
	int page;
} instruction;

/* Private Functions */
void readInstructionFile(string filename);
void readRandomFile(string randfile);
void initSystem(string name, int size, string rand);

/* Private Instances */
vector<INSTRUCTION> instrList;
vector<int> randVals;
FaultHandler* sys;



int main(int argc, char *argv[]) {
	
	/* ---------------------- */
	/* Part I: Initialization */
	/* ---------------------- */
	int O = 0;  // 1 for printing out instructions, 0 otherwise.
	int P = 0;  // 1 for printing out P, 0 otherwise.
	int F = 0;  // 1 for printing out F, 0 otherwise.
	int S = 0;  // 1 for printing out S, 0 otherwise.
	string algo = "l";             // "l" by default
	int PHYSICAL_SPACE_SIZE = 32;  // 32 by default
	
	// Step 1: parse command line
	vector<string> paras;
	for (int i=0; i< argc; i++) {
		paras.push_back(string(argv[i]));	
	}
	vector<string>::iterator it;
	for (int i=0; i<argc; i++) {
		if (paras[i].find("-a") != string::npos){
			algo = paras[i].substr(2);
		}
		if (paras[i].find("-f") != string::npos){
			PHYSICAL_SPACE_SIZE = atoi(paras[i].substr(2).c_str());	
		}
		if (paras[i].find("-o") != string::npos) {
			string s = paras[i].substr(2);
			if (s.find("O") != string::npos) 
				O = 1;
			if (s.find("P") != string::npos) 
				P = 1;
			if (s.find("F") != string::npos) 
				F = 1;
			if (s.find("S") != string::npos) 
				S = 1;
		}
	}
	string filename = paras[paras.size()-2];
	string randfile = paras.back();
	
	// Step 2: read in instruction file and 
	// 		   intialize system. 
	readInstructionFile(filename);
	initSystem(algo, PHYSICAL_SPACE_SIZE, randfile);
	long long unmapCt = 0;
	long long mapCt = 0;
	long long inCt = 0;
	long long outCt = 0;
	long long zeroCt = 0;
	unsigned long long cost = 0;
	bool OO = false;
	sys->initialize();
	
	/* --------------------- */
	/* Part II: SIMULATION   */
	/* --------------------- */
	// Read in instruction file
	unsigned long long counter = 0;
	unsigned long long instruSize = instrList.size();
	int type;
	int page;
	int oldPage;
	int newFrame;
	while ( counter < instruSize ) {
		// Step 1: get instruction
		instruction = instrList[counter];
		type = instrList[counter].type;
		page = instrList[counter].page;
		if ( O == 1) {
			//cout << "==> inst: " << type << " " << page << endl;
			printf("==> inst: %d %d\n", type, page);
		}
		
		// Step 2: if the page is in physical frame, upate mapping, goes to next turn
		// 		   otherwise, deal with this page.
		if ( !sys->isPresent(page) ) {
			/* If page is not present, check frame table.
			 * If frame table has available frame, get a free one.
			 * Otherwise, replacement algorithm will select an old
			 * page to remove, then make room for the coming new page.
			 */
			sys->setModified(type, page);
			if ( sys->hasFree() ){
				newFrame = sys->getFreeFrame();
			} else {
				oldPage = sys->getOldPage();
				newFrame = sys->getFreshFrame();
				if ( O == 1) {
					printf("%llu: UNMAP %3d %3d\n", counter, oldPage, newFrame );
				}
				sys->unmap(oldPage);
				unmapCt++;
				if ( sys->isDirty(oldPage)) {
					if ( O == 1) {
						printf("%llu: OUT%4d %3d\n", counter, oldPage, newFrame);
					}
					outCt++;	
				}
				sys->resetModified(oldPage);
			}

			if ( sys->hasPagedout(page) ) {
				if ( O == 1) {
					printf("%llu: IN %6d %3d\n", counter,page, newFrame);
				}
				inCt++;
			} else {
				if ( O == 1) {
					printf("%llu: ZERO %8d\n", counter, newFrame);
				}
				zeroCt++;
			}

			if ( O == 1) {
				printf("%llu: MAP%6d%4d\n", counter, page, newFrame);
			}
			sys->setMap(newFrame, page);
			mapCt++;
		} else {
			sys->updateMap(type, page);
		}

		if (OO == true) {
			sys->printPageTable();
			sys->printFrameTable();
			sys->printSequence();
		}
		counter++;
	}
	
	if (P==1) {	sys->printPageTable();}
	if (F==1) { 
		sys->printFrameTable();
		printf("\n");
	}
	if (S==1) {
		cost = (unmapCt+mapCt) * 400 + (inCt+outCt) * 3000 + zeroCt * 150 + instruSize;
		printf("SUM %llu U=%llu M=%llu I=%llu O=%llu Z=%llu ===> %llu\n",
			instruSize, unmapCt, mapCt, inCt, outCt, zeroCt, cost);
	}
		
	return 0;
}

/* Private Functions */

/*
 * Line starts with # is comment and should be ignored.
 * For uncommented lines, it has two integers,
 * the first is R/W, 0 for READ and  1 for WRITE;
 * the second is virtual page index.
 */
void readInstructionFile(string filename) {
	/*pte.present = 0;*/
	/*pte.pagedout = 0;*/
	int type, index;
	ifstream input;
	input.open(filename.c_str());
	string line;
	while (getline(input, line)) {
		if (line[0] == '#') continue;
		istringstream tokens(line);
		tokens >> type >> index; 
		instruction.type = type;
		instruction.page= index;
		instrList.push_back(instruction);
	}
	input.close();
}

void readRandomFile(string randfile){
	int scale;
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

void initSystem(string name, int size, string rand){
	if (name == "a")
		sys = new HandlerAging(size,1);
	if (name == "A") 
		sys = new HandlerAging(size,2);
	if (name == "l") 
		sys = new HandlerLRU(size);
	if (name == "N") {
		readRandomFile(rand);
		sys = new HandlerNRU(size, randVals);
	}
	if (name == "r") {
		readRandomFile(rand);
		sys = new HandlerRandom(size, randVals);
	}
	if (name == "f")
		sys = new HandlerFIFO(size);
	if (name == "s") 
		sys = new HandlerSC(size);
	if (name == "c") 
		sys = new HandlerClock(size, 1);
	if (name == "C")
		sys = new HandlerClock(size, 2);
}
