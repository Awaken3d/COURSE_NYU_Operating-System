/* File:: SchedulerFSCAN.h
 * ----------------------
 */

#ifndef _schedulerfscan_h
#define _schedulerfscan_h

#include "SchedulerBase.h"

class SchedulerFSCAN : public SchedulerBase {
public:
	/* Constructors */
	SchedulerFSCAN();

	/* Member Functions */
	Operation getOperation();
	void setOperation(int t);
	bool isEmpty();
	void deleteOperation();
	int getSize();

	/* Helper Functions */
	void printTracks();
	bool checkDirection();

private:	
	int m_dir;
	int m_pos;
    int m_on; // 0 for up; 1 for down	
	int m_track;
	int m_size;
	vector<Operation> m_up;
	vector<Operation> m_down;
	vector<Operation>::iterator m_it;
};
#endif


