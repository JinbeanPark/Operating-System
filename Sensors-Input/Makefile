#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

FILES=lab4b.c smoketest.sh README Makefile

default:
	gcc -g -lm -lmraa -o lab4b -Wall -Wextra lab4b.c

check: lab4b ./smoketest.sh
	chmod 777 smoketest.sh
	./smoketest.sh
clean: 
	rm -f lab4b lab4b-805330751.tar.gz

dist: 
	tar -zcvf lab4b-805330751.tar.gz $(FILES)
