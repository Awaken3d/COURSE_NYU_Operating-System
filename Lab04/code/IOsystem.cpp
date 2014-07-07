/* File: IOsystem.cpp
 * ------------------
 */

#include "IOsystem.h"

/* Constructors */
IOsystem::IOsystem(){
}

/* Member Functions */
void IOsystem::addEvent(Operation oper) {
	m_list.push_back(oper);
}

/*
 * setEvent: create new operation, change type and ft
 * put this operation into list, 
 * so that operations in  list are ordered
 * by their finish time.
 */
void IOsystem::setEvent(int t) {
	// 1. create new event
	oper.type  = 2;
	if (t>0) { 
		oper.ft = oper.st + t; 
	} else {
		oper.ft = oper.st - t; 
	}

	// 2. find position for new event
	list<Operation>::iterator it;
	for (it=m_list.begin(); it!=m_list.end(); ++it) {
		if (oper.ft < it->ft) break;
	}
	m_list.insert(it, oper);
} 

Operation IOsystem::getEvent() {
	return m_list.front();
}

void IOsystem::deleteEvent() {
	m_list.erase(m_list.begin());
}

bool IOsystem::isEmpty() {
	return m_list.size() == 0;
}

void IOsystem::setFinish() {
	for(m_it=m_finish.begin(); m_it!=m_finish.end(); ++m_it) {
		if (oper.op < m_it->op) {
			break;
		}
	}
	m_finish.insert(m_it, oper);
}

void IOsystem::printFinish() {
	printf("IOREQS INFO\n");
	for (m_it=m_finish.begin(); m_it!=m_finish.end();++m_it) {
		printf("%5d: %5d %5d %5d\n", 
			m_it->op, m_it->at, m_it->st, m_it->ft);
	}	
}

void IOsystem::printSum(int curTime, int totalMove){
	//printf("SUM: %d %d %.2lf %.2lf %d\n",
	int waitTime = 0;
	int turnAround = 0;
	int maxWait = 0;
	int wait = 0;
	int size;
	for(m_it = m_finish.begin(); m_it != m_finish.end(); ++m_it) {
		turnAround += m_it->ft - m_it->at;
		wait = m_it->st - m_it->at;
		if (wait > maxWait) maxWait = wait;
		waitTime += wait;	
	}
	size = m_finish.size();
	printf("SUM: %d %d %.2lf %.2lf %d\n",
			curTime,totalMove,
		 double(turnAround)/double(size),
		 (double(waitTime)/double(size)), 
		 maxWait);
}

/* Helper Functions */
int IOsystem::getSize() {
	return m_list.size();
}
