#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

FILES=lab4c_tcp.c lab4c_tls.c Makefile README

default: 
	gcc -g -Wall -Wextra -lm -lmraa lab4c_tcp.c -o lab4c_tcp
	gcc -g -Wall -Wextra -lm -lmraa -lssl -lcrypto lab4c_tls.c -o lab4c_tls

clean:
	rm -f lab4c_tcp lab4c_tls lab4c-805330751.tar.gz

dist:
	tar -zcvf lab4c-805330751.tar.gz $(FILES)
