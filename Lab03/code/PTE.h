
#ifndef _pte_h
#define _pte_h

//using namespace std;
struct PTE {
	unsigned present     :1;
	unsigned modified    :1;
	unsigned referenced  :1;
	unsigned pagedout    :1;
	unsigned frameIndex  :28;
};
#endif
