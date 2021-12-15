Project 1A Terminal I/O and Inter-Process Communication

NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751

lab1a-805330751.tar.gz includes following files.

1. lab1a.c
1) lab1a.c is a single C source module.
2) lab1a.c is a program that build a multi-process telnet-like client and
   server
3) lab1a.c has two mode non shell mode and shell mode (--shell=program).
   (1) Program supports character-at-a-time, full duplex terminal I/O.
   (2) Program supports polled I/O and passing input and output between two processes.
4) lab1a.c exploits the following OS features:
   (1) Terminal I/O and modes (as an example of a complex API set)
   (2) Polled I/O (a meas of awaiting input from multiple sources)
   (3) Inter-process Communication
   (4) Exception handling
5) lab1a.c develops a multi-process application
6) lab1a.c develops debugging skills for multi-process and non-deterministic problems.


2. Makefile
Compile the following targets

1) default: Build the lab1a executable with the gcc -g -Wall -Wextra lab1a.c -o lab1a.
2) clean: Delete all files created by the Makefile, and return the directory to its freshly
   untarred state.
3) dist: Build the distribution tarball.


3. Cited sources
1) https://ccle.ucla.edu/pluginfile.php/3382673/mod_resource/content/1/Week%202.pdf
2) https://d1b10bmlvqabco.cloudfront.net/attach/k80p2njhzz42vb/icx548hthaz3r1/k8uq8btjtq2a/Week_2_Discussion_CS_111.pdf