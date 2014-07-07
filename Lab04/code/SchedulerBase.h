/* File: SchedulerBase.h
 * ---------------------
 */

#ifndef _schedulerbase_h
#define _schedulerbase_h

#include <list>
#include <vector>
#include <algorithm>
#include <cstdio>
#include "Operation.h"

using namespace std;

class SchedulerBase {
protected:
	//list<Operation> m_issue;	

public:
	/* Constructor */
	SchedulerBase();

	/* Member Functions */
	virtual void setOperation(int t) = 0;
	virtual bool isEmpty() = 0;
	virtual Operation getOperation() = 0;
	virtual void deleteOperation() = 0;
	virtual int getSize() = 0;
	/* Helper Functions */

private:
	
};
#endif
