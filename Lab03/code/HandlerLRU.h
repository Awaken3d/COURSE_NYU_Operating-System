#ifndef _handlerlru_h 
#define _handlerlru_h 

#include "FaultHandler.h"
#include <list>

class HandlerLRU : public FaultHandler {

public:
	HandlerLRU();
	HandlerLRU(int);
	void setMap(int newFrame, int page);
	int getFreshFrame();
	void updateMap(int type, int page);

private:
	int getOldPage();	
	list<int> m_list;
	void addToList(int page);
	int m_return;
};

#endif
