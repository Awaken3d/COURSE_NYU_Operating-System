#ifndef _handlerfifo_h
#define _handlerfifo_h

#include "FaultHandler.h"

class HandlerFIFO : public FaultHandler {
public:

	HandlerFIFO();
	HandlerFIFO(int);

	int getFreshFrame();


private:
	int getOldPage();
};

#endif
