Project 2B Lock Granularity and Performance

NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751

lab2b-805330751.tar.gz includes following files.

1. lab2_list.c
1) The source for a C program that implements the specified command line
   options (--threads, --iterations, --yield, --sync, --lists), drives
   one or more parallel threads that do operations on a shared linked list,
   and reports on the final list and performance.


2. SortedList.h
1) Header file describing the interfaces for linked list operations.


3. SortedList.c
1) C module that implements insert, delete, lookup, and length methods for
   a sorted doubly linked list.


4. lab2b_list.csv
1) Containing all of results for all of test runs.


5. Makefile
Compile the following targets

1) default: Compile the program lab2_list with the -Wall and -Wextra options.

2) tests: running all specified test cases to generate CSV results.

3) profile: running tests with profiling tools to generate an execution profiling report.

3) graphs: Using gnuplot to generate the required graphs

4) dist: creating the deliverable tarball.

5) clean: Deleting all programs and output created by the Makefile.


6. lab2_list.gp
1) Generating data reduction graphs for the multi-threaded list project.


7. profile.out
1) Execution profiling report showing where time was spent in the un-partitioned
   spin-lock implementation.


8. lab2b_1.png
1) Throughput vs number of threads for mutex and spin-lock synchronized list operations.


9. lab2b_2.png
1) Mean time per mutex wait and mean time per operation for mutext-synchronized list operations.


10. lab2b_3.png
1) Successful iterations vs threads for each synchronizaton method.


11. lab2b_4.png
1) Throughput vs number of threads for mutex synchronized partitioned lists.


12. lab2b_5.png
1) Throughput vs number of threads for spin-lock-synchronized partitioned lists.


Cited sources
https://ccle.ucla.edu/pluginfile.php/3458005/mod_resource/content/0/Week%206.pdf
https://ccle.ucla.edu/pluginfile.php/3458690/mod_resource/content/0/Week6.pdf
https://piazza.com/redirect/s3?bucket=uploads&prefix=attach%2Fk80p2njhzz42vb%2Ficx548hthaz3r1%2Fk9yp51szb7eg%2FWeek_6_Discussion_CS_111.pptx


Questions
1) Questions 2.3.1 - CPU time in the basic list implementation:
(1) Where do you believe most of the CPU time is spent in the 1 and 2-thread list tests?
=> For the 1 and 2 thread list tests, I believe most of the CPU time is spent 
   in list operations rather than synchronization. One of the reason is 
   that the time of waiting for lock is not much in 1 or 2-threads.

(2) Why do you believe these to be the most expensive parts of the code?
=> Spending in list operations is the most expensive in the 1 and 2-thread list tests
   because it does not need to spend most of time waiting for releasing locks
   in small number of threads compared to large number of threads, so
   spending in list operations should be the most expensive parts in the 1
   and 2-thread list tests.

(3) Where do you believe most of the CPU time is being spent in the high-thread
    spin-lock tests?
=> I believe most of the CPU time in high-thread is spent in spinning,
   waiting for releasing lock.

(4) Where do you believe most of the CPU time is being spent in the high-thread
    mutex tests?
=> I believe most of the CPU time in the high-thread mutext tests is spent 
   in list operations which are insert, delete, look up, and getting the 
   length of list because many threads which could not get the lock would
   go to sleep until releasing lock, so most of the CPU time in the high-threads
   mutex will be spent in list operations instead of spinning, waiting for releasing
   locks.


2) Question 2.3.2 - Execution profiling
(1) Where (what lines of code) are consuming most of the CPU time when the
    spin-lock version of the list exerciser is run with a large number of
    threads?
=> while (__sync_lock_test_and_set(lockS + hashKey, 1));
   while (__sync_lock_test_and_set(lockS + i, 1));
   
   Most of the CPU time in spin-lock version with larger number of threads
   consume in spinning, waiting for releasing locks.

(2) Why does this operation become so expensive with large numbers of threads?
=> The reason why this operation become so expensive with larger number of 
   threads because the more the number of threads increase, the more 
   the total time of spinning and waiting for releasing lock increase. 
   In other words, contention among threads gets increased as increasing 
   the number of threads.


3) Question 2.3.3 - Mutex Wait Time
(1) Why does the average lock-wait time rise so dramatically with the number
    of contending threads?
=> The reason why the average lock-wait time rise so dramatically with the 
   number of contending threads because the more the number of contending
   threads increase, the longer it takes time for each contending threads
   to get the one same lock. In other words, many contending threads wait 
   for getting one same lock, so the average lock-wait time increase 
   dramatically with the number of contending threads.

(2) Why does the completion time per operation rise (less dramatically) 
    with the number of contending threads?
=> The first reason is that the more the number of threads increase, 
   the more it takes time to wait and get the lock, but at least one 
   threads having lock still perform, so it makes less dramatically 
   rise the time per operation with the number of contending threads.
   The second reason is that the more the number of threads increase,
   the more the context switching occur, so it also affect rising the time
   per operation with the number of contending threads.

(3) How is it possible for the wait time per operation to go up faster (or
    higher) than the completion time per operation?
=> For the completion time per operation, it counts operations 
   in whole threads with the completion time of whole threads.
   In contrast, the wait time per operation counts operations of 
   one thread with the wait time of the relevant thread.
   These thing considered, the wait time per operation go up faster 
   than the completion time per operation.
   Therefore, the more threads increase, the more the difference between 
   the wait time per operation and the completion time per operation increase.


4) Question 2.3.4 - Performance of partitioned Lists.
(1) Explain the change in performance of the synchronized methods as a function
    of the number of lists.
=>  The more the number of lists increases, the more the performance 
    (=Throughput) of the synchronized methods increases.

(2) Should the throughput continue increasing as the number of lists is further
    increased? If not, explain why not.
=> No, the throughput will not be continuing increasing as the number of lists is
   further increased because if the number of lists reaches the level that each
   element owns their sublist, each threads does not have to wait for releasing lock.
   As a result, the throughput (=performance) will be maximized 
   and no longer the performance will be increased at that moment.

(3) It seems reasonable to suggest the throughput of an N-way partitioned list should
    be equivalent to the throughput of a single list with fewer (1 / N) threads.
    Does this appear to be true in the above curves? If not, explain why not.
=> It looks like similar, but does not looks exactly same,
   so I think it does not appear to be true in the curves.
   The reason is why it does not appear to be true in the curves
   because the more the number of sublists increases, the more the size of
   each sublist decreases. Therefore, it is highly possible that the time
   of spending in list operations (Insert, delete, look up, length) significantly
   decrease in each sublist. As a result, the throughput of a single list 
   with fewer (N/1)threads would be lower than the throughput of a N-way 
   partitioned list.