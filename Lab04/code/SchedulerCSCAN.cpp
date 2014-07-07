/* File: SchedulerCSCAN.cpp
 * -----------------------
 */

#include "SchedulerCSCAN.h"

/* Constructors */
SchedulerCSCAN::SchedulerCSCAN(){
	m_dir = 0;
	m_pos = 0;
}

/* Member Functions */
/* 
 * setOperation: set operation in order of its track
 */
void SchedulerCSCAN::setOperation(int t){
	// 1. create new operation
	oper.ft = t;

	// 2. find right position for new operation
	for(m_it = m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		if (oper.track < m_it->track) break;
	} 
	m_issue.insert(m_it, oper);
	printTracks();
}

/* getOperation: if there is higher track request, select that; 
 * otherwise select from the beginning
 */
Operation SchedulerCSCAN::getOperation() {
	if (m_issue.back().track >= m_track) {
		for (m_it=m_issue.begin(); m_it!=m_issue.end();++m_it){
			if (m_it->track >= m_track) break;
		}
	} else {
		m_it = m_issue.begin();		
	}
	m_track = m_it->track;
	return *m_it;
}

bool SchedulerCSCAN::isEmpty() {
	return m_issue.size() == 0;
}

void SchedulerCSCAN::deleteOperation() {
	m_issue.erase(m_it);
	printTracks();
	//printf("current track: %d\n", m_track);
}

int SchedulerCSCAN::getSize() {
	return m_issue.size();
}

/* Helper Functions */
void SchedulerCSCAN::printTracks() {
	if (false) {
	printf("tracks: ");
	for (m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		printf("%d ", m_it->track);
	}
	printf(" || CT: %d DIR: %d \n", m_track, m_dir);
	}
}

bool SchedulerCSCAN::checkDirection() {
	// 1. if goes upwards, have higher track request?
	if (m_dir == 0) {
		return m_issue.back().track < m_track;
	} else {
		return m_issue.front().track > m_track;
	}
}

