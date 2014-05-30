/* File: Scheduler.h
 * -----------------------------------------------------------
 * This class includes SchedulerFCFS (First Come First Serve), 
 * SchedulerLCFS (Last Come First Serve), SchedulerSJF 
 * (Shortest Job First), SchedulerRR (Round Robin).
 * -----------------------------------------------------------
 * By  : Jingxin Zhu
 * Date: 05/28/2014
 */
#ifndef _scheduler_h
#define _scheduler_h

#include <queue>
#include <stack>
#include <list>

#include "Event.h"

using namespace std;

class Scheduler{
    public:
        virtual void addReady(Event ready) = 0;
        virtual bool isEmpty() = 0;
        virtual Event getReady() = 0;
    private:
};

class SchedulerFCFS : public Scheduler {
    public:
        /* Constructor */
        SchedulerFCFS();	

        /* Member Functiuons */
        void addReady(Event process);
        bool isEmpty();
        Event getReady();

    private:
        queue<Event> m_ready;
};

class SchedulerLCFS: public Scheduler {
    public:
        /* Constructor */
        SchedulerLCFS();	

        /* Member Functiuons */
        void addReady(Event process);
        bool isEmpty();
        Event getReady();

    private:
        stack<Event> m_ready;
};

class SchedulerRR : public Scheduler {
    public:
        /* Constructor */
        SchedulerRR(int quantum);	
        int quantum;

        /* Member Functiuons */
        void addReady(Event process);
        bool isEmpty();
        Event getReady();

    private:
        queue<Event> m_ready;
};

class SchedulerSJF : public Scheduler {
    public:
        /* Constructor */
        SchedulerSJF();	

        /* Member Functiuons */
        void addReady(Event process);
        bool isEmpty();
        Event getReady();

    private:
        list<Event> m_ready;
};

#endif
