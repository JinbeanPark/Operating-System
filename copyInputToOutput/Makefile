#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

FILES=lab0.c quick_smoke_test.sh test1.txt backtrace.png breakpoint.png README Makefile

default:
	gcc -g -Wall -Wextra lab0.c -o lab0

check:
	./quick_smoke_test.sh

clean:
	rm -f lab0 lab0-805330751.tar.gz

dist:
	tar -zcvf lab0-805330751.tar.gz $(FILES)