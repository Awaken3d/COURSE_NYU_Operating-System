/* File: SchedulerFSCAN.cpp
 * -----------------------
 */

#include "SchedulerFSCAN.h"

/* Constructors */
SchedulerFSCAN::SchedulerFSCAN(){
	m_dir = 0;
	m_pos = 0;
	m_on  = -1;
}

/* Member Functions */
/* 
 * setOperation: set operation in order of its track
 */
void SchedulerFSCAN::setOperation(int t){
	// 1. create new operation
	oper.ft = t;

	// 2. find right position for new operation
	if (m_on == -1 || m_on == 1){
		for(m_it = m_up.begin(); m_it!=m_up.end(); ++m_it) {
			if (oper.track < m_it->track) break;
		} 
		m_up.insert(m_it, oper);
		// !!! add two items in the beginning???
		if (m_on == -1) m_on = 0;
	} else {
		for(m_it = m_down.begin(); m_it!=m_down.end(); ++m_it) {
			if (oper.track < m_it->track) break;
		} 
		m_down.insert(m_it, oper);
	}
	printTracks();
}

/* getOperation: if there is higher track request, select that; 
 * otherwise select from the beginning
 */
Operation SchedulerFSCAN::getOperation() {

	//printf("on: %d\n", m_on);
	if (m_on==0 && m_up.size()==0) {
		//printf(" up to down \n");
		m_on = 1;
		printTracks();
		m_dir = 0;
	} else{
		//printf("down's size: %d \n", int(m_down.size()));
		//printf("on: %d\n", m_on);
		if (m_on==1 && m_down.size()==0) {
			//printf(" down to up \n");
			m_on = 0;
			printTracks();
			m_dir = 0;
		}
	}

	if (checkDirection()) {
		//printf("turn around!\n");
		m_dir = 1 - m_dir;
	}

	//printf("dir:: %d\n", m_dir);
	if (m_on == 0) {

		if (m_dir == 0) {
		for (m_it=m_up.begin(); m_it!=m_up.end(); ++m_it){
			if (m_it->track >= m_track) break;
		}
		oper = *m_it;	
		} 
		if (m_dir == 1) {
			m_pos = m_up.size();
			while (true) {
				m_pos--;
				if (m_up[m_pos].track <= m_track) {
					for (m_it=m_up.begin(); m_it!=m_up.end();++m_it){
					if (m_it->track == m_up[m_pos].track) break;
				}
				break;
			}
			}
		oper = *m_it;
		}
	} 
	
	if (m_on == 1) {

		if (m_dir == 0) {
		for (m_it=m_down.begin(); m_it!=m_down.end(); ++m_it){
			if (m_it->track >= m_track) break;
		}
		oper = *m_it;	
		} 
		if (m_dir == 1) {
			m_pos = m_down.size();
			while (true) {
				m_pos--;
				if (m_down[m_pos].track <= m_track) {
					for (m_it=m_down.begin(); m_it!=m_down.end();++m_it){
					if (m_it->track == m_down[m_pos].track) break;
				}
				break;
			}
			}
		oper = *m_it;
		}
	} 
	m_track = m_it->track;
	return *m_it;
}

bool SchedulerFSCAN::isEmpty() {
	return m_up.size() == 0 && m_down.size()==0;
}

void SchedulerFSCAN::deleteOperation() {
	if (m_on==0){
		m_up.erase(m_it);
	} else {
		m_down.erase(m_it);
	}
	printTracks();
	//printf("current track: %d\n", m_track);
}

int SchedulerFSCAN::getSize() {
	return m_up.size();
}

/* Helper Functions */
void SchedulerFSCAN::printTracks() {
	if (false) {
	printf("tracks: ");
	if (m_on ==0){
	for (m_it=m_up.begin(); m_it!=m_up.end(); ++m_it) {
		printf("%d ", m_it->track);
	}
		printf(" || CT: %d DIR: %d Q: %d \n", m_track, m_dir, m_on);
	} else {
		for (m_it=m_down.begin(); m_it!=m_down.end(); ++m_it) {
			printf("%d ", m_it->track);
		}
		printf(" || CT: %d DIR: %d Q: %d \n", m_track, m_dir, m_on);
	}
	}
}

bool SchedulerFSCAN::checkDirection() {
	// 1. if goes upwards, have higher track request?
	if (m_dir == 0) {
		if (m_on == 0) {
			return m_up.back().track < m_track;
		} else {
			return m_down.back().track < m_track;
		}
	} else {
		if (m_on == 0) {
			return m_up.front().track > m_track;
		} else {
			return m_down.front().track > m_track;
		}
	}
}


