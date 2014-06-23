#ifndef	_handleraging_h 
#define	_handleraging_h 

#include "FaultHandler.h"
#include "AGEBIT.h"

class HandlerAging : public FaultHandler {
public:

	HandlerAging();
	HandlerAging(int,int);

	int getFreshFrame();
	void printSequence();


private:
	int m_type;
	int m_page;
	int m_counter;
	int getOldPage();
	void initAgeVector();
	void updateAgeVector();
	void resetRbit();
	void printAgeVector(int m_page);
	int findMinBit();
	int findPage(int page);
	vector<AGEBIT> m_age;
};

#endif

