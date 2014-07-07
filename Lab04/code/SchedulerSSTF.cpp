/* File: SchedulerSSTF.cpp
 * -----------------------
 */

#include "SchedulerSSTF.h"

/* Constructors */
SchedulerSSTF::SchedulerSSTF(){
	m_track = 0;
}

/* Member Functions */
void SchedulerSSTF::setOperation(int t){
	oper.ft = t;
	m_issue.push_back(oper);
}

/* getOperation: calculate the distance between each element and current track, select the element with the smallest distance
 */
Operation SchedulerSSTF::getOperation() {
	int temp;
	int pos = 0;
	if (m_issue.front().track > m_track) {
		m_dis = m_issue.front().track - m_track;
	} else {
		m_dis = m_track - m_issue.front().track;
	}
	m_minit = m_issue.begin();
	for(m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it){
		if (m_it->track > m_track) {
			temp = m_it->track - m_track;
		} else {
			temp = m_track - m_it->track;
		}
		if (temp < m_dis) {
			m_dis = temp;
			m_pos = pos;
			m_minit = m_it;
		}
		pos++;
	}
	
	m_track = m_minit->track;
	return *m_minit;	
}

void SchedulerSSTF::deleteOperation() {
	m_issue.erase(m_minit);
	printTracks();
}

bool SchedulerSSTF::isEmpty() {
	return m_issue.size() == 0;
}

int SchedulerSSTF::getSize() {
	return m_issue.size();
}

/* Helper Functions */
void SchedulerSSTF::printTracks() {
	if (false) {
	printf("tracks: ");
	for (m_it=m_issue.begin(); m_it!=m_issue.end(); ++m_it) {
		printf("%d ", m_it->track);
	}
	printf(" || CT: %d \n", m_track);
	}
}

