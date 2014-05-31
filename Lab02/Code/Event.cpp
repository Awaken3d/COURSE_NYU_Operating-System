/* File : Event.cpp
 * ------------------
 * By   : Jingxin Zhu
 * Date : 03/28/2014
 */


#include "Event.h"

/* Constructor */
Event::Event(int pid, int type, int AT, int TC, int CB, int IB) {

    this->pid= pid;
    this->AT = AT;
    this->TC = TC;
    this->CB = CB;
    this->IB = IB;
    m_type = type;

    defineMode(type);	

    ts = AT;	// time for event to execute
    m_ts = AT;  // time for creating old event
    m_remain= TC;
    m_dur = 0;
    m_cb  = 0;
    m_ib  = 0;
    m_sum = 0;
    m_cbSum = 0;
    m_ibSum = 0;
}

/* Member Functions */

void Event::recordTime() {
    m_ts = ts;
}

int Event::getTS() const {
    return ts;
}

bool Event::isFinish(){
    return m_remain==0;
}

void Event::setType(int type){
    m_type = type;
    // RUNNG-> BLOCK: type 1
    // READY-> READY: type 2 
    // READY-> RUNNG: type 3 
    // BLOCK-> READY: type 4 
    // RUNNG-> READY: type 5
    defineMode(type);
}

void Event::defineMode(int type) {
    if (type == 1) {
        m_fromMode = "RUNNG";
        m_toMode = "BLOCK";
        m_mode = m_toMode;
    }
    if (type == 2){
        m_fromMode = "READY";
        m_toMode = "READY";
        m_mode = m_toMode;
    }
    if (type == 3){
        m_fromMode = "READY";
        m_toMode = "RUNNG";
        m_mode = m_toMode;
    }
    if (type == 4){
        m_fromMode = "BLOCK";
        m_toMode = "READY";
        m_mode = m_toMode;
    }
    if (type == 5) {
        m_fromMode = "RUNNG";
        m_toMode = "READY";
        m_mode = "PREEMPT";
    }
}

void Event::setCPUburst(int n) {
    if (m_remain < n) {
        m_cb = m_remain;
    } else {
        m_cb = n;
    }
    m_cbSum += m_cb;
}

void Event::setIOburst(int n) {
    m_ib = n;
    m_ibSum += m_ib;	
}

void Event::setRemain(int type, int burst){
    if (type == 3 || type == 5) {
        m_remain -= burst;
    }
}

void Event::printDone() {
    m_dur = ts - m_ts;
    string mode = "BLOCK";
    cout << endl;
    cout <<  "==> " << ts << " " << pid << " " 
        <<	"ts=" << m_ts << " " <<  mode << "  dur=" << m_dur << endl;  
    cout << "==> T(" << pid << "): Done" << endl; 
}

int Event::getType() const{
    return m_type;
}

int Event::getDuration() {
    m_dur = ts - m_ts;
    return m_dur;
}

int Event::getRemain() const {
    return m_remain;
}

int Event::getCPUburst() const {
    return m_cb;
}

int Event::getIOburst() const {
    return m_ib;
}

string Event::getMode() const {
    return m_mode;
}

// Overload operator <<
ostream& operator << (ostream& out, Event& p) {
    string mode = p.getMode();
    out << endl;
    out <<  "==> " << p.ts << " " << p.pid << " " 
        <<	"ts=" << p.m_ts << " " <<  mode 
        << "  dur=" << p.getDuration() << endl;  
    out << "T(" << p.pid << ":" << p.ts<< "): " << p.m_fromMode << " -> " 
        << p.m_toMode;
    if ( mode == "READY" ) {}
    if ( mode == "RUNNG" || p.getType() == 5)
        out << "  cb=" << p.getCPUburst() << " rem=" << p.getRemain();
    if (mode == "BLOCK") {
        // if done; otherwise
        out << "  ib=" << p.getIOburst() << " rem=" << p.getRemain();
    }
    return out;
}
