#include "HandlerRandom.h"

HandlerRandom::HandlerRandom() {

}

HandlerRandom::HandlerRandom(int ftsize, vector<int> rand) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
	m_rand = rand;	
	m_size = rand.size();
	m_ofs = 0;
}

int HandlerRandom::getOldPage() {
	m_counter = m_rand[m_ofs] % m_ftsize;
	m_ofs++;
	if (m_ofs == m_size) m_ofs = 0;
	return ft[m_counter];
}

int HandlerRandom::getFreshFrame() {
	return m_counter;
}

