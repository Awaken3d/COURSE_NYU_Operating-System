/* File: IOsystem.h
 * -----------------
 */
#ifndef _iosystem_h
#define _iosystem_h

#include <list>
#include <cstdio>

#include "Operation.h"
using namespace std;

class IOsystem{
public:
	/* Constructors */
	IOsystem();

	/* Member Functions */
	void addEvent(Operation oper);
	void setEvent(int t);
	Operation getEvent();
	void deleteEvent();
	bool isEmpty();
	void setFinish();
	void printFinish();
	void printSum(int curTime, int totalMove);

	/* Helper Functions */	
	int getSize();

private:
	list<Operation> m_list;
	list<Operation> m_finish;
	list<Operation>::iterator m_it;
};
#endif 
