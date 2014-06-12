Goal:

This lab aims to simulate Discrete Event Simulator (DES), scheduling a batch of processes with different schedule algorithms (First Come First Serve, Last Come First Serve, Shortest Job First, and Round Robin). Please see Assignment_Lab02_Scheduling.pdf for detailed requirements.


Compile:

‘Code’ Directory contains all required files for compiling. Makefile is included, thus typing ‘make’ in terminal can compile. Executable file ‘sched.out’ should be created after compiling.


Test:
Put ‘sched.out’ into BatchTest directory (For easy use, I have already put it there). In BatchTest directory, type in terminal:

	sh ./runit.sh
        sh ./diffit.sh . ../ValidationData

This will take input files in BatchTest directory, generate results,  and compare them with
expected results in ValidationData directory.
