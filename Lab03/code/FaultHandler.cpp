#include "FaultHandler.h"

void FaultHandler::initialize() {
	PTE pte;
	pte.present  = 0;
	pte.pagedout = 0;
	pte.modified = 0;
	for (int i=0; i<64; i++) {
		pt.push_back(pte);
	}
}

bool FaultHandler::isPresent(int page) {
	return pt[page].present == 1;
}

bool FaultHandler::hasFree() {
	int s = ft.size();
	return m_ftsize != s;
}

int FaultHandler::getFreeFrame() {
	return ft.size();
}

bool FaultHandler::isDirty(int page) {
	if (pt[page].modified == 1) {
		pt[page].pagedout = 1;
	} 
	return pt[page].modified == 1;
}

bool FaultHandler::hasPagedout(int page) {
	return pt[page].pagedout == 1;
}

void FaultHandler::setModified(int type, int page) {
	if (type == 1) {
		pt[page].modified = 1;
	}
}

void FaultHandler::setMap(int newFrame, int page) {
	pt[page].frameIndex = newFrame;
	pt[page].present = 1;
	pt[page].referenced = 1;
	if ( hasFree() ) {
		ft.push_back(page);
	} else {
		ft[newFrame] = page;
	}
}

void FaultHandler::printPageTable() {
	for (int i=0; i< m_ptsize; i++) {
		if (pt[i].present== 1){
			// print pte
			printPTE(i);
		} else {
			if (pt[i].pagedout == 1) {
				//cout << "# " ;
				printf("# ");
			} else {
				//cout << "* ";	
				printf("* ");
			}	
		}
	}	
	printf("\n");
}

void FaultHandler::printPTE(int index) {
	//cout << index << ":";
	printf("%d:",index);
	if (pt[index].referenced == 1) {
		//cout << "R";
		printf("R");
	} else {
		cout << "-";
	}
	if (pt[index].modified == 1){
		cout << "M";
	} else {
		cout << "-";
	}
	
	if (pt[index].pagedout == 1){
		cout << "S ";
	} else {
		cout << "- ";
	}
}

void FaultHandler::printFrameTable() {
	int s = ft.size();	
	for (int i=0; i<s; i++){
		//cout << ft[i] << " ";	
		printf("%d ",ft[i]);
	}
	for (int i=s; i<m_ftsize; i++){
		//cout << "* ";
		printf("* ");
	}
}

void FaultHandler::resetModified(int page) {
	pt[page].modified = 0;
}

void FaultHandler::updateMap(int type, int page) {
	if (type==1){
		pt[page].modified = 1;
	}
	pt[page].referenced = 1;
}

void FaultHandler::unmap(int oldPage) {
	pt[oldPage].present = 0;
}

void FaultHandler::printSequence() {
	cout << " || ";
	int i = 0;
	int s= ft.size();
	for (i=m_counter; i<s; i++) {
		cout << i << " ";
	}
	for (i=0; i<m_counter; i++) {
		cout << i << " ";
	}
	cout << endl;
}

