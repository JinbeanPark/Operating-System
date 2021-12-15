#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

FILES=lab1b-client.c lab1b-server.c README Makefile

default:
	gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client
	gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server

client:
	gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client

server:
	gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server
	
clean:
	rm -f lab1b-client lab1b-server lab1b-805330751.tar.gz

dist:
	tar -zcvf lab1b-805330751.tar.gz $(FILES)