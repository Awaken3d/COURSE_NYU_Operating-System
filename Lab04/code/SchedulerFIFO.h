/* File: SchedulerFIFO.h
 * ---------------------
 */

#ifndef _schedulerfifo_h
#define _schedulerfifo_h
#include "SchedulerBase.h"

using namespace std;

class SchedulerFIFO : public SchedulerBase{
public:
	/* constructors */
	SchedulerFIFO();

	/* member functions */
	Operation getOperation();
	void setOperation(int t);
	bool isEmpty();
	void deleteOperation();
	int getSize();

	/* helper functions */
	void printTracks();

private: 
	list<Operation> m_issue;
	list<Operation>::iterator m_it;
	int m_track;
};
#endif
