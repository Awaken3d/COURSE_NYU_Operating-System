#include "HandlerFIFO.h"

HandlerFIFO::HandlerFIFO() {

}

HandlerFIFO::HandlerFIFO(int ftsize) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
}

int HandlerFIFO::getFreshFrame() {
	int result = m_counter;
	m_counter++;
	if (m_counter == m_ftsize) {
		m_counter = 0;
	}
	return result;
}

int HandlerFIFO::getOldPage() {
	return ft[m_counter];
}


