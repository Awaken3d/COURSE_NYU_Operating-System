/* File: Scheduler.cpp
 * -----------------------
 * By  : Jingxin Zhu
 * Date: 05/28/2014
 */
#include "Scheduler.h"

using namespace std;

/* SchedulerFCFS: First Come First Serve */
SchedulerFCFS::SchedulerFCFS() : Scheduler() {
}

void SchedulerFCFS::addReady(Event event){
    m_ready.push(event);
}

bool SchedulerFCFS::isEmpty() {
    return m_ready.size() == 0;
}

Event SchedulerFCFS::getReady() {
    Event process = m_ready.front();
    m_ready.pop();
    return process;
}

/* SchedulerLCFS: Last Come First Serve */
SchedulerLCFS::SchedulerLCFS() : Scheduler() {
}

void SchedulerLCFS::addReady(Event event){
    m_ready.push(event);
}

bool SchedulerLCFS::isEmpty() {
    return m_ready.size() == 0;
}

Event SchedulerLCFS::getReady() {
    Event process = m_ready.top();
    m_ready.pop();
    return process;
}

/* SchedulerSJF: Shortest Job First */
SchedulerSJF::SchedulerSJF() : Scheduler() {
}

void SchedulerSJF::addReady(Event event){
    list<Event>::iterator it;
    for (it=m_ready.begin(); it!=m_ready.end();++it) {
        if (event.getRemain() < it->getRemain()) {
            break;
        }
    }
    m_ready.insert(it, event);
}

bool SchedulerSJF::isEmpty() {
    return m_ready.size() == 0;
}

Event SchedulerSJF::getReady() {
    Event process = m_ready.front();
    m_ready.erase(m_ready.begin());
    return process;
}

/* SchedulerRR: Round Robin */
SchedulerRR::SchedulerRR(int quantum) : Scheduler() {
    this->quantum = quantum;
}

void SchedulerRR::addReady(Event event){
    m_ready.push(event);
}

bool SchedulerRR::isEmpty() {
    return m_ready.size() == 0;
}

Event SchedulerRR::getReady() {
    Event process = m_ready.front();
    m_ready.pop();
    return process;
}
