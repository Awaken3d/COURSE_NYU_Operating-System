#ifndef _handlerclock_h
#define _handlerclock_h

#include "FaultHandler.h"

class HandlerClock : public FaultHandler {
public:
	HandlerClock();
	HandlerClock(int,int);

	int getFreshFrame();
	void printSequence();

private:
	int m_type;
	int getOldPage();
	int m_hand;
	void resetRbit();

};

#endif
