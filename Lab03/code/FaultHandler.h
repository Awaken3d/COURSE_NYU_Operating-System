#ifndef _faulthandler_h
#define _faulthandler_h

#include <vector>
#include <iostream>
#include <algorithm>
#include "stdio.h"

#include "PTE.h"

using namespace std;

class FaultHandler {

protected:
	vector<PTE> pt;
	vector<int> ft;
	int m_ptsize;
	int m_ftsize;
	int m_counter;

public:
	
	int getFreeFrame();
	bool isPresent(int page);
	bool isDirty(int page);
	bool hasFree();
	bool hasPagedout(int page);
	void initialize();
	void setModified(int type, int page);
	void resetModified(int page);
	void printPageTable();
	void printFrameTable();
	
	virtual int getFreshFrame() = 0;
	virtual int getOldPage() = 0;
	virtual void unmap(int oldPage);
	virtual void printSequence();
	virtual void setMap(int newFrame, int page);
	virtual void updateMap(int type, int page);

private:
	void printPTE(int index);
};

#endif
