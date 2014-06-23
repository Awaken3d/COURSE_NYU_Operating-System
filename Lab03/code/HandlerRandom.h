#ifndef _handlerrandom_h 
#define _handlerrandom_h 

#include "FaultHandler.h"



class HandlerRandom : public FaultHandler {

public:
	HandlerRandom();
	HandlerRandom(int , vector<int> );

	int getFreshFrame();

private:
	int getOldPage();
	vector<int> m_rand;	
	int m_ofs;
	int m_size;
};

#endif
