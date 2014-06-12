Goal:
    
This program implements a linker in Operating System. Detailed requirements please see lab1-linker.pdf.

Compile:

Makefile is in included, so typing in terminal 'make' is enough to compile. Then executable file 'Lab01' is created.
    
Test: 

Put 'Lab01' into BatchTest directory (For easy use, I have already done this). In BatchTest directory, type in terminal:

    sh ./runit.sh
    sh ./diffit.sh . .

First command will take input files in BatchTest directory, generate result files (result-1, result-2, result-3,...), and compare them with expected results(out-1, out-2, out-3, ...). If anything is different, it will show in console; otherwise, nothing happens.

    

