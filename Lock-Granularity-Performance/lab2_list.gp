#! /usr/bin/gnuplot
#
#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#    8. Average wait-for-lock
#
# output:
#	lab2b_1.png ... Throughput vs Threads with mutex and spin-lock synchronized list operations.
#	lab2b_2.png ... Average wait-for-lock & run time per operation against the number of competing threads.
#	lab2b_3.png ... Correctness of partition the single resource into multiple independent resources.
#	lab2b_4.png ... Aggregated throughput with mutex and partitioned lists.
#    lab2b_5.png ... Aggregated throughput with spin-locks and partitioned lists.


# general plot parameters
set terminal png
set datafile separator ","


# lab2b_1.png
set title "Throughput vs Threads with mutex and spin-lock synchronized list operations"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operation / sec)"
set logscale y 10
set output 'lab2b_1.png'
oneBill = 1000000000

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'Mutex synchronized list operations' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'Spin-lock synchronized list operations' with linespoints lc rgb 'green'


# lab2b_2.png
set title "Average wait-for-lock & run time per operation against the number of competing threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Mean time for wait-for-lock & Mean run time per operation" 
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Wait-for-lock time' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Run time per operation' with linespoints lc rgb 'green'


# lab2b_3.png
set title "Correctness of partition the single resource into multiple independent resources"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Sucessful iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Unprotected' with points lc rgb 'red', \
     "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Spin-lock' with points lc rgb 'green', \
     "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Mutex' with points lc rgb 'blue'


# lab2b_4.png
set title "Aggregated throughput with mutex and partitioned lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operation / sec)"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=16' with linespoints lc rgb 'yellow'  


# lab2b_5.png
set title "Aggregated throughput with spin-locks and partitioned lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operation / sec)"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=8' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(oneBill/($7)) \
	title 'lists=16' with linespoints lc rgb 'yellow'