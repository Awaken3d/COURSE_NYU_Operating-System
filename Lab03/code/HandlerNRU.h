#ifndef _handlernru_h 
#define _handlernru_h 

#include "FaultHandler.h"


class HandlerNRU : public FaultHandler {
public:

	HandlerNRU();
	HandlerNRU(int, vector<int>);
	void setMap(int newFrame, int page);
	void updateMap(int type, int page);
	int getFreshFrame();
	void printSequence();


private:
	int m_request; // number of 
	vector<int> m_rand;	
	vector< vector<int> > m_matrix;
	int m_ofs; // ofs counter for random number
	int m_size; // size of random file
	int m_i;
	int m_j;
	void initVector();
	void reset();
	void addToMatrix(int row, int page);
	int getOldPage();
	vector<int>::iterator m_it;
};

#endif
