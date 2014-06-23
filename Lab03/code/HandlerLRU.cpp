#include "HandlerLRU.h"

HandlerLRU::HandlerLRU() {

}

HandlerLRU::HandlerLRU(int ftsize) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
}

int HandlerLRU::getOldPage() {
	m_return = m_list.front();
	m_list.pop_front();
	return m_return;
}

int HandlerLRU::getFreshFrame() {
	vector<int>::iterator it;
	it = std::find(ft.begin(), ft.end(), m_return);
	return std::distance(ft.begin(), it);
}

void HandlerLRU::setMap(int newFrame, int page) {
	pt[page].frameIndex = page;
	pt[page].present = 1;
	pt[page].referenced = 1;
	if ( hasFree() ) {
		ft.push_back(page);
		m_list.push_back(page);
	} else {
		ft[newFrame] = page;
		addToList(page);
	}
}

void HandlerLRU::updateMap(int type, int page) {
	if (type==1){
		pt[page].modified = 1;
	}
	pt[page].referenced = 1;
	addToList(page);
}

void HandlerLRU::addToList(int page) {
	m_list.remove(page);
	m_list.push_back(page);
}


