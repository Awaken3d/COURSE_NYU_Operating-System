#include "HandlerSC.h"

HandlerSC::HandlerSC() {

}

HandlerSC::HandlerSC(int ftsize) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
}

int HandlerSC::getOldPage() {
	// check that page's reference bit,
	// if it is 0, select that page;
	// otherwise, check next page
	// if all are referenced, select as 
	// FIFO;
	int page; 
	int i = 0;
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

int HandlerSC::getFreshFrame() {
	int page = m_counter;
	m_counter++;
	if (m_counter == m_ftsize) m_counter = 0;
	return page;
}


