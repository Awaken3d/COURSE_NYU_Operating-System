/* File: Operation.h
 * ------------------
 */
#ifndef _operation_h
#define _operation_h

struct Operation{
	int op;   // operand
	int at;   // arrival time
	int st;   // start time
	int ft;   // end time
	int type; // type of operation
	int track;// track requested

};
extern Operation oper;
#endif
