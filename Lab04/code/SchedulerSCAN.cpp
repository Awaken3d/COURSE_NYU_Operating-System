/* File: SchedulerSCAN.cpp
 * -----------------------
 */

#include "SchedulerSCAN.h"

/* Constructors */
SchedulerSCAN::SchedulerSCAN(){
	m_dir = 0;
	m_pos = 0;
}

/* Member Functions */
/* 
 * setOperation: set operation in order of its track
 */
void SchedulerSCAN::setOperation(int t){
	// 1. create new operation
	oper.ft = t;

	// 2. find right position for new operation
	for(m_it = m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		if (oper.track < m_it->track) break;
	} 
	m_issue.insert(m_it, oper);
	printTracks();
}

Operation SchedulerSCAN::getOperation() {
	// 1. need turn around ?
	if (checkDirection()) {
		//printf("turn around!\n");
		m_dir = 1 - m_dir;
	}
	printTracks();
	// 2. get next request, next higher if goes upwards
	// next lower if goes downwards
	if (m_dir == 0) {
		for (m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it){
			if (m_it->track >= m_track) break;
		}
		oper = *m_it;	
	} 
	if (m_dir == 1) {
		m_pos = m_issue.size();
		while (true) {
			m_pos--;
			if (m_issue[m_pos].track <= m_track) {
				for (m_it=m_issue.begin(); m_it!=m_issue.end();++m_it){
					if (m_it->track == m_issue[m_pos].track) break;
				}
				break;
			}
		}
		oper = *m_it;
	}
	m_track = oper.track;
	return oper;
}

bool SchedulerSCAN::isEmpty() {
	return m_issue.size() == 0;
}

void SchedulerSCAN::deleteOperation() {
	if (m_dir==0) {
		m_issue.erase(m_it);
	} else {
		m_issue.erase(m_it);
	}
	printTracks();
	//printf("current track: %d\n", m_track);
}

int SchedulerSCAN::getSize() {
	return m_issue.size();
}

/* Helper Functions */
void SchedulerSCAN::printTracks() {
	if (false) {
	printf("tracks: ");
	for (m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		printf("%d ", m_it->track);
	}
	printf(" || CT: %d DIR: %d \n", m_track, m_dir);
	}
}

bool SchedulerSCAN::checkDirection() {
	// 1. if goes upwards, have higher track request?
	if (m_dir == 0) {
		return m_issue.back().track < m_track;
	} else {
		return m_issue.front().track > m_track;
	}
}
