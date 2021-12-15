Project 0 Warm-Up

NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751

lab0-805330751.tar.gz includes following files.

1. lab0.c
1) lab0.c is a single C source module.
2) lab0.c is a program that copies its standard input to its standard output
  by reading from file descriptor 0 and writing to file descriptor 1.
3) Include the following options
--input=filename: use the specified file as standard input
--output=filename: create the specified file and use it as standard output
--segfault: force a sgmentation fault
--catch: use signal to register a SIGSEGV handler that catches the segmentation
  fault, logs an error message.


2. Makefile
Compile the following targets

1) default: Build the lab0 executable with the -g -Wall and -Wextra options.
2) check: Runs a quick smoke-test on whether or not the program seems to work,
   supports the required arguments, and properly reports success or failure.
   Following is checklist that I included in my smoke-test.
   (1) Check if the program seems to work or not.
   (2) Check if the program supports the the required arguments.
   (3) Check if the program successfully copies the standard input to standard output.
   (4) Check if the program catchs the case of not existing input file.
   (5) Check if the program catches the case of copying the standard input to output file not having write permisson.
   (6) Check if the segfault and catch do works well or not.
   (7) Deleted all files created.
3) clean: Delete all files created by the Makefile, and return the directory to its freshly
   untarred state.
4) dist: Build the distribution tarball.


3. quick_smoke_test.sh
Check whether or not the program seems to work, supports the required arguments,
and properly reports success or failure.
Following is checklist that I included in the quick_smoke-test.
   (1) Check if the program seems to work or not.
   (2) Check if the program supports the the required arguments.
   (3) Check if the program successfully copies the standard input to standard output.
   (4) Check if the program catchs the case of not existing input file.
   (5) Check if the program catches the case of copying the standard input to output file not having write permisson.
   (6) Check if the segfault and catch do works well or not.
   (7) Deleted all files created.

4. test1.txt
test1.txt is a file that I made to check if my program sucessfully copies
its standard input to its standard output.

Following is the contents of test1.txt:

Jinbean Park

Test version for CS 111.

Spring Quarter 2020.

5. backtrace.png
1) Screen snapshot from a gdb session.
2) Showing a segfault and associated stack-trace.

6. breakpoint.png
1) Screen snapshot from a gdb session.
2) Showing a breakpoint and variable inspection.

7. Cited sources
1) https://linux.die.net/man/2/read
2) https://linux.die.net/man/3/getopt_long
3) https://12bme.tistory.com/224
4) https://mintnlatte.tistory.com/285
5) https://twpower.github.io/79-usage-of-memset-function
6) https://webdir.tistory.com/154