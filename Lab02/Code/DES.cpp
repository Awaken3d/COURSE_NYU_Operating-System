/* File:DES.cpp
 * -------------------
 * By   : Jingxin Zhu
 * Date : 05/30/2014
 * -------------------
 */

#include "DES.h"
#include "Event.h"
#include <list>
using namespace std;

/* Constructor */
DES::DES(){
}

/* Member Functions */
void DES::setEvent(Event& p) {
    // type 3 event will not be put into DES
    insertList(p);	
}  

void DES::deleteEvent() {
    m_EventBase.erase(m_EventBase.begin());
}

// Get a new event 
Event DES::getEvent() {
    return m_EventBase.front();
} 

bool DES::isEmpty() {
    return (m_EventBase.size()==0);
}

void DES::insertList(Event& p){
    list<Event>::iterator it;
    for (it=m_EventBase.begin(); it!=m_EventBase.end();++it) {	
        if (p.ts < it->ts) {
            break;
        }
    }
    m_EventBase.insert(it, p);
}

void DES::createEvent(int type, Event& e) {
    switch (type) { 
        case 1:  // RUNNG -> BLOCK
            e.setType(4); break;
        case 2:  // READY -> READY
            e.setType(3); break;
        case 3:  // READY -> RUNNG
            e.setType(1); break;
        case 4:  // BLOCK -> READY
            e.setType(3); break;
        case 5:  // RUNNG -> READY
            e.setType(3); break;
        default:
            break;
    }
}

void DES::addCompleteEvent(Event event){
    list<Event>::iterator it;
    for (it=m_completeEvent.begin(); it!=m_completeEvent.end();++it) {
        if (event.pid < it->pid) {
            break;
        }
    }
    m_completeEvent.insert(it,event);	
}

list<Event> DES::getCompleteProcess() {
    return m_completeEvent;
}

void DES::printSummary(int curTime, int cpuTime, int ioTime) {
    list<Event>::iterator it;
    int TTsum = 0;
    int CPUwaiting;
    int waitSum = 0;
    for (it=m_completeEvent.begin(); it!=m_completeEvent.end();++it) {
        TTsum += it->ts - it->AT;
        CPUwaiting = it->ts - it->AT - it->TC - it->m_ibSum;
        waitSum += CPUwaiting;
        printf("%04d: %4d %4d %4d %4d | %4d %4d %4d %4d\n",
                it->pid, it->AT, it->TC, it->CB, it->IB, 
                it->ts, (it->ts-it->AT), it->m_ibSum, CPUwaiting);
    }
    double proCt = double(m_completeEvent.size());
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", 
            curTime,
            // percentage of time at least one process is running	
            double((100*cpuTime/double(curTime))),
            // percentage of time at least one process is performing IO
            100 * double( double(ioTime) / double(curTime)) ,  
            // average time for completing one process from arrival to finishing
            double(double(TTsum)/ proCt),
            // average cpu waiting time among processes
            double( double(waitSum) / proCt),
            // number of events that can be finished per 100 time units
            double(100 * proCt / double(curTime) ));
}
