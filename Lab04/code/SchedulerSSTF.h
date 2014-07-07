/* File: SchedulerSSTF.h
 * ---------------------
 */

#ifndef _schedulersstf_h 
#define	_schedulersstf_h 
#include "SchedulerBase.h"

using namespace std;

class SchedulerSSTF : public SchedulerBase{
public:
	/* constructors */
	SchedulerSSTF();

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
	list<Operation>::iterator m_minit;
	int m_track;
	int m_dis;
	int m_pos;
	vector<int> m_distance;
	vector<int>::iterator m_disit;
};
#endif

