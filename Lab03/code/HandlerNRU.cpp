#include "HandlerNRU.h"

HandlerNRU::HandlerNRU() {

}

HandlerNRU::HandlerNRU(int ftsize, vector<int> rand) {
	m_ptsize = 64;
	m_ftsize = ftsize;
	m_counter = 0;
	m_rand = rand;	
	m_size = rand.size();
	m_ofs = 0;
	initVector();
	m_request = 1;
}

int HandlerNRU::getOldPage() {
	if (m_request == 10) {
		//cout << " @@ reset NRU refbits while walking PTE" << endl;
		for(m_it=ft.begin(); m_it!=ft.end(); ++m_it) {
			pt[*m_it].referenced = 0;
		}
	}

	m_i = 0;
	while ( m_i < 4) {
		if (m_matrix[m_i].size() != 0) {
			break;
		}
		m_i++;
	}
	m_j = m_rand[m_ofs] % m_matrix[m_i].size();
	/*
	 *cout << " @@ lowest_class=" << m_i
	 *<< ": selidx=" << m_j << " from"; 
	 *for (m_it=m_matrix[m_i].begin(); m_it != m_matrix[m_i].end();++m_it){
	 *    cout << " " << *m_it;
	 *}
	 *cout << endl;
	 */
	m_counter = m_matrix[m_i][m_j]; 
	m_matrix[m_i].erase(m_matrix[m_i].begin() + m_j);
	if (m_request == 10) {
		//cout << "here" << endl;
		reset();
		m_request=0;
	}
	m_request++;
	m_ofs++;
	if (m_ofs == m_size) m_ofs = 0;
	return m_counter;
}

int HandlerNRU::getFreshFrame() {
	m_it= std::find(ft.begin(), ft.end(), m_counter);	
	return std::distance(ft.begin(), m_it);
}

void HandlerNRU::initVector() {
	vector<int> a;
	for (int i=0; i< 4; i++) {
		m_matrix.push_back(a);
	}
}

void HandlerNRU::setMap(int newFrame, int page) {
	pt[page].referenced = 1;
	pt[page].frameIndex = page;
	pt[page].present = 1;
	if ( hasFree() ) {
		ft.push_back(page);
	} else {
		ft[newFrame] = page;
	}
	if (pt[page].modified == 0) {
		addToMatrix(2,page);
	} else {
		addToMatrix(3,page);
	}	
}

void HandlerNRU::addToMatrix(int row, int page) {
	for (m_it=m_matrix[row].begin(); m_it!=m_matrix[row].end();++m_it){
		if (page < *m_it) {
			break;
		}
	}
	m_matrix[row].insert(m_it, page);
}

void HandlerNRU::printSequence() {
	cout << endl;
}

void HandlerNRU::reset() {
	int s;
	int page;
	for (int i=2; i<4; i++){
		s = m_matrix[i].size();
		for (int j=0; j<s; j++) {
			page = m_matrix[i][j];
			pt[page].referenced = 0;
			if (pt[page].modified == 0) {
				addToMatrix(0, page);
			} else {
				addToMatrix(1, page);
			}
		}
	}
 	m_matrix[2].clear();
	m_matrix[3].clear();
}

void HandlerNRU::updateMap(int type, int page) {
	int r,m,row;
	r = pt[page].referenced;
	m = pt[page].modified;
	if (r==0 && m==0) row = 0;
	if (r==0 && m==1) row = 1;
	if (r==1 && m==0) row = 2;
	if (r==1 && m==1) row = 3;
   	m_it = std::find(m_matrix[row].begin(), m_matrix[row].end(), page);
	if (r==0){
		m_matrix[row].erase(m_it);
		if ( type==0 && m==0) {
			addToMatrix(2,page);
		} 
		if ( type==0 && m==1) {
			addToMatrix(3,page);
		}
		if ( type==1 && m==0){
			pt[page].modified = 1;
			addToMatrix(3,page);
		}
		if ( type==1 && m==1) {
			addToMatrix(3,page);
		} 
	} else {
		if ( type==1 && m==0){
			pt[page].modified = 1;
			m_matrix[row].erase(m_it);
			addToMatrix(3,page);
		}
	}	
	pt[page].referenced = 1;
}
