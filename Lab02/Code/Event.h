/* File : Event.h
 * ------------------
 * By   : Jingxin Zhu
 * Date : 03/28/2014
 */
#ifndef _event_h 
#define _event_h 

#include <string>
#include <iostream>
#include <list>

using namespace std;

class Event{
    public:
        /* Constructor */
        Event(int pid, int type, int AT, int TC, int CB, int IB);

        int pid; // process id
        int AT;  // arriving time
        int TC;  // 
        int CB;  // cpu burst
        int IB;  // I/O burst
        int ts;

        int m_remain; // time to complete remaining part of process
        int m_dur;    // duration time from last mode to current mode
        int m_cb; // current cpu burst time
        int m_ib; // current io burst time
        int m_cbSum; // accumulated cpu burst time for this process
        int m_ibSum; // accumulated io burst time for this proces
        int m_sum;   // accumulated cpu and io burst time
        int m_ts; // beginning time of last mode
        int m_type;
        string m_fromMode;
        string m_toMode;
        string m_mode;

        /* Member Functions */
        bool isFinish();
        void setCPUburst(int rand);
        void setIOburst(int rand);
        void printDone();
        void setType(int type);
        void setRemain(int type, int burst);
        void defineMode(int type);
        void recordTime();
        int getTS();

        int getType();
        int getDuration();
        int getRemain();
        int getCPUburst();
        int getIOburst();
        string getMode();

    private:
};

ostream& operator <<(ostream& out, Event& p);

#endif
