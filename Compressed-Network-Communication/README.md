Project 1B Compressed Network Communication

NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751

lab1b-805330751.tar.gz includes following files.

1. lab1b-client.c
1) lab1b-client.c is a single C source module.
2) lab1b-client.c program opens a connection to a server rather than sending
   it directly to a shell. The client should then send input from the keyboard
   to the socket (while echoing to the display), and input from the socket to
   the display.
3) Maintaining the same non-canonical (character at a time, no echo) terminal 
   behavior you used in Project 1A.
4) Including, in the client, a --log=filename option, which maintains a record of 
   data sent over the socket.


2. lab1b-server.c
1) The server program connects with the client, receive the client's 
   commands and send them to the shell, and will "serve" the client 
   the outputs of those commands.
2) The server program listens on a network socket 
   (port specified with the mandatory --port= command line parameter).
3) Accept a connection when it is made.
4) Once a connection is made, the server program forks a child process,
   which execs a shell (program specified with the --shell= option)
   to process commands received; the server process communicates with the shell via pipes
5) Input received from the shell pipes is forwarded out to the network socket.


3. Makefile
Compile the following targets

1) default: Build the lab1b-client and lab1b-server executable 
   with the gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client
   and gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server.

2) client: Build the lab1b-client executable
   with the gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client 

3) server: Build the lab1b-server executable 
   with the gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server.

4) clean: Delete all files created by the Makefile, and return the directory to its freshly
   untarred state.

5) dist: Build the distribution tarball.


4. Cited sources
file:///Users/jinbean/Desktop/UCLA/2020%20Spring%20quarter/CS%20111/Project/Project%201B/P1B.html
https://ccle.ucla.edu/pluginfile.php/3403397/mod_resource/content/0/Week%203.pdf
https://ccle.ucla.edu/pluginfile.php/3403251/mod_resource/content/0/Week3.pdf
https://piazza.com/redirect/s3?bucket=uploads&prefix=attach%2Fk80p2njhzz42vb%2Ficx548hthaz3r1%2Fk94q8hunzvqw%2FWeek_3_Discussion_CS_111.pptx
https://www.zlib.net/manual.html
https://refspecs.linuxbase.org/LSB_3.0.0/LSB-Core-generic/LSB-Core-generic/zlib-deflate-1.html