/* File:: SchedulerSCAN.h
 * ----------------------
 */

#ifndef _schedulerscan_h
#define _schedulerscan_h

#include "SchedulerBase.h"

class SchedulerSCAN : public SchedulerBase {
public:
	/* Constructors */
	SchedulerSCAN();

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
	vector<Operation> m_down;
	vector<Operation> m_up;
	vector<Operation>::iterator m_it;
	list<Operation>::reverse_iterator m_reit;
	
};
#endif
