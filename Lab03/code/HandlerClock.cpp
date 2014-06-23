#include "HandlerClock.h"

HandlerClock::HandlerClock() {

}

HandlerClock::HandlerClock(int ftsize, int type) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
	m_hand = 0;
	m_type = type;
}

int HandlerClock::getOldPage() {
	// check that page's reference bit,
	// if it is 0, select that page;
	// otherwise, check next page
	// if all are referenced, select as 
	// FIFO;
	int page; 
	int i = 0;
	// for clock
	if (m_type == 1) {
		while (i < m_ftsize) {
			page = ft[m_counter];
			if (pt[page].referenced==0) break;
			pt[page].referenced = 0;
			m_counter++;
			if (m_counter == m_ftsize) {
				m_counter = 0;
			} 
			i++;
		}
		return ft[m_counter];
	} 

	// for Clock 
	/* From hand position, look for next 'present' page
	 * if next is not present, move on, if moves to the end;
	 * turn around from the beginning of the page table. 
	 */
	else {
		while (true) {
			if (pt[m_hand].present == 1){
				if(pt[m_hand].referenced == 0) {
					break;
				}
				pt[m_hand].referenced = 0;
			} 
			m_hand++;
			if (m_hand == m_ptsize) {
				m_hand = 0;
			}
		}
		

		return m_hand;
	}
}

int HandlerClock::getFreshFrame() {
	if (m_type == 1) {
		int page = m_counter;
		m_counter++;
		if (m_counter == m_ftsize) m_counter = 0;
	 	return page;
	} else {
		int page = pt[m_hand].frameIndex;
		//resetRbit();
		m_hand++;
		if (m_hand == m_ptsize) m_hand = 0;
		return page;
	}
}

void HandlerClock::printSequence() {
	cout << " || hand = " << m_hand << endl;
}

void HandlerClock::resetRbit() {
	for (int i=0; i<m_ptsize; i++) {
		if (pt[i].present == 1) {
			pt[i].referenced = 0;
		}
	}
}
