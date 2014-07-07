/* File: SchedulerFIFO.cpp
 * -----------------------
 */

#include "SchedulerFIFO.h"

/* Constructors */
SchedulerFIFO::SchedulerFIFO(){
	m_track = 0;
}

/* Member Functions */
void SchedulerFIFO::setOperation(int t){
	oper.ft = t;
	m_issue.push_back(oper);
}

Operation SchedulerFIFO::getOperation() {
	m_track = m_issue.front().track;
	return m_issue.front();	
}

void SchedulerFIFO::deleteOperation() {
	m_issue.erase(m_issue.begin());
	printTracks();
}

bool SchedulerFIFO::isEmpty() {
	return m_issue.size() == 0;
}

int SchedulerFIFO::getSize() {
	return m_issue.size();
}

/* Helper Functions */
void SchedulerFIFO::printTracks() {
	if (false) {
	printf("tracks: ");
	for (m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		printf("%d ", m_it->track);
	}
	printf(" || CT: %d \n", m_track);
	}
}
