/* File: DES.h
 * ---------------------------------------------
 * Descrete Event Simulation (DES) class acts as 
 * event manager, createing, defining, and 
 * executing events.
 * 
 * By   : Jingxin Zhu
 * Date : 05/30/2014 
 * ---------------------------------------------
 */

#ifndef _des_h	
#define _des_h 

#include <list>
#include <stdio.h>

#include "Event.h"

using namespace std;

class DES {
    public:
        /* constructor */
        DES(); 

        /* Member Functions */	
        void createEvent(int type, Event& e);
        void setEvent(Event& p);
        void deleteEvent();
        void addCompleteEvent(Event event);
        Event getEvent();
        bool isEmpty();
        list<Event> getCompleteProcess();
        void printSummary(int curTime, int cpuTime, int ioTime);
        
    private:
        list<Event> m_EventBase;
        list<Event> m_completeEvent;
        void insertList(Event& p);

};

#endif
