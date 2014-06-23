#include "HandlerAging.h"

HandlerAging::HandlerAging() {

}

HandlerAging::HandlerAging(int ftsize, int type) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
	initAgeVector();
	m_type = type;
}

int HandlerAging::getFreshFrame() {
	if (m_type == 2) {
		int pos = std::find(ft.begin(), ft.end(), m_page) - ft.begin();
		m_age[pos].age = 0;
		return pos;
	} else {
		m_age[m_page].age = 0;
		return m_page;
	}
}

int HandlerAging::getOldPage() {
	// 1. right shift 1 bit then 
	//    add reference value to leftmost bit
	updateAgeVector();	
	// 2. reset reference bit to 0
	resetRbit();
	// 3. find the minimum agebit
	m_page = findMinBit();
	// 4. find the smallest page which
	//    has agebit found in last step
	if (m_type == 2) { // "a"
		m_page = findPage(m_page);
		// printAgeVector(m_page);
		return m_page ; 
	} else {
		//printAgeVector(m_page);
		return ft[m_page];
	}
}

void HandlerAging::initAgeVector() {
	AGEBIT a;
	a.age = 0;
	for (int i=0; i<m_ftsize; i++) {
		m_age.push_back(a);	
	}
}

void HandlerAging::updateAgeVector() {
	for (int i=0; i< m_ftsize; i++) {
		m_age[i].age >>= 1;
		if (pt[ft[i]].referenced == 1) {
			m_age[i].age |= 0x80000000;
		} else {
			m_age[i].age |= 0;
		}
	}
}

void HandlerAging::resetRbit() {
	for (int i=0; i<m_ftsize; i++) {
		pt[ft[i]].referenced = 0;
	}
}

int HandlerAging::findMinBit() {
	int result = 0;
	unsigned long long m_min = m_age[0].age;
	for (int i=1; i<m_ftsize; i++) {
		if (m_age[i].age < m_min ) {
			result = i;
			m_min = m_age[i].age;
		}
	}
	return result;
}

int HandlerAging::findPage(int page) {
	int result = ft[page];
	for (int i=0; i<m_ftsize; i++) {
		if ( m_age[i].age == m_age[page].age ) {
			if ( ft[i] < result) {
				result = ft[i];
			} 
		} 
	}
	return result;	
}

void HandlerAging::printSequence() {
	if (m_type == 2) {
		cout << " ||";
		for (int i=0; i<m_ptsize; i++) {
			if (pt[i].present == 1) {
				int pos = std::find(ft.begin(), ft.end(), i) - ft.begin();
				printf(" %d:%x", i, int(m_age[pos].age));
			} else {
				cout << " *";
			}
		}
	} 
	if (m_type == 1) {
		cout << " || ";
		int s = ft.size();
		for (int i=0; i<s; i++) {
			//if (pt[ft[i]].present == 1){
				printf("%d:%x ", i, int(m_age[i].age));
			//}
		}	
	}
	cout << endl;
}

void HandlerAging::printAgeVector(int page) {
	cout << " @@ ";
	if (m_type == 2){
	for (int i=0; i<m_ptsize; i++) {
		if (pt[i].present == 1) {
			int pos = std::find(ft.begin(), ft.end(), i) - ft.begin();
			printf(" %d:%x", i, int(m_age[pos].age));
		} else {
			cout << " *";
		}
	}
	cout << endl;

	int pos = std::find(ft.begin(), ft.end(), page) - ft.begin();
	cout << " @@ min_pte = "
	<< page;
	printf(" age=%x", int(m_age[pos].age));
	cout << endl;
	} else {
		for (int i=0; i<m_ftsize; i++) {
			printf(" %d:%x", i, int(m_age[i].age));
		}
		printf("\n @@ min_frame =%2d age=%x\n", page, int(m_age[page].age));
	}

}


