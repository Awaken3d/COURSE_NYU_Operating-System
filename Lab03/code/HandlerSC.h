#ifndef _handlersc_h
#define _handlersc_h

#include "FaultHandler.h"

class HandlerSC : public FaultHandler {

public:
	HandlerSC();
	HandlerSC(int);

	int getFreshFrame();

private:
	int getOldPage();	
};

#endif
