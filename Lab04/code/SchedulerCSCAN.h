/* File:: SchedulerCSCAN.h
 * ----------------------
 */

#ifndef _schedulercscan_h
#define _schedulercscan_h

#include "SchedulerBase.h"

class SchedulerCSCAN : public SchedulerBase {
public:
	/* Constructors */
	SchedulerCSCAN();

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
	int m_track;
	int m_size;
	vector<Operation> m_issue;
	vector<Operation>::iterator m_it;
};
#endif

