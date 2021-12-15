#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

FILES=lab2_list.c README Makefile SortedList.c SortedList.h lab2b_list.csv lab2_list.gp profile.out lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png

default:
	gcc -g -Wall -Wextra -lpthread -lprofiler lab2_list.c SortedList.c -o lab2_list

tests: default
	rm -f lab2b_list.csv
	# lab2b_1.png & lab2b_2.png
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24  --iterations=1000 --sync=s >> lab2b_list.csv
	
	# lab2b_3.png
	-./lab2_list --threads=1  --iterations=1 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=2 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=4 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=8 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=16 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=2 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=4 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=8 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=2 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=4 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=8 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=2 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=4 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=8 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=16 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=1 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=2 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=4 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=8 --yield=id --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=16 --yield=id --list=4 >> lab2b_list.csv
	
	-./lab2_list --threads=1  --iterations=10 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=20 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=40 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=80 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=10 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=20 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=40 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=80 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=10 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=20 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=40 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=80 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=10 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=20 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=40 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=80 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=10 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=20 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=40 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=80 --yield=id --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=10 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=20 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=40 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=80 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=10 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=20 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=40 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=80 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=10 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=20 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=40 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=80 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=10 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=20 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=40 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=80 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=10 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=20 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=40 --yield=id --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=16  --iterations=80 --yield=id --sync=m --list=4 >> lab2b_list.csv
	
	#lab2b_4.png
	#-./lab2_list --threads=1  --iterations=1000 --sync=s --list=1 >> lab2_list.csv
	#-./lab2_list --threads=2  --iterations=1000 --sync=s --list=1 >> lab2_list.csv
	#-./lab2_list --threads=4  --iterations=1000 --sync=s --list=1 >> lab2_list.csv
	#-./lab2_list --threads=8  --iterations=1000 --sync=s --list=1 >> lab2_list.csv
	#-./lab2_list --threads=12  --iterations=1000 --sync=s --list=1 >> lab2_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=s --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --list=8 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --list=8 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --list=8 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --list=8 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=s --list=8 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=s --list=16 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=s --list=16 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=s --list=16 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=s --list=16 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=s --list=16 >> lab2b_list.csv
	#-./lab2_list --threads=1  --iterations=1000 --sync=m --list=1 >> lab2_list.csv
	#-./lab2_list --threads=2  --iterations=1000 --sync=m --list=1 >> lab2_list.csv
	#-./lab2_list --threads=4  --iterations=1000 --sync=m --list=1 >> lab2_list.csv
	#-./lab2_list --threads=8  --iterations=1000 --sync=m --list=1 >> lab2_list.csv
	#-./lab2_list --threads=12  --iterations=1000 --sync=m --list=1 >> lab2_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=m --list=4 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=m --list=8 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --list=8 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --list=8 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --list=8 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=m --list=8 >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1000 --sync=m --list=16 >> lab2b_list.csv
	-./lab2_list --threads=2  --iterations=1000 --sync=m --list=16 >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1000 --sync=m --list=16 >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1000 --sync=m --list=16 >> lab2b_list.csv
	-./lab2_list --threads=12  --iterations=1000 --sync=m --list=16 >> lab2b_list.csv
	
profile: default
	rm -f profLab2.gperf profile.out
	#LD_PRELOAD=/u/cs/ugrad/jinbean/CS111/project2B/gperftools-2.7.90/lib/libprofiler.so.0
	LD_PRELOAD=/u/cs/ugrad/jinbean/lib/libprofiler.so.0
	CPUPROFILE=profLab2.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
	pprof --text ./lab2_list ./profLab2.gperf > profile.out
	pprof --list=t_function ./lab2_list ./profLab2.gperf >> profile.out
	rm -f lab2_list profile.gperf ./profLab2.gperf

graphs: tests
	./lab2_list.gp

dist: graphs profile
	tar -zcvf lab2b-805330751.tar.gz $(FILES)

clean:
	rm -f lab2_list lab2b-805330751.tar.gz