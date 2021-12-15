Project 2A Races and Synchronization

NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751

lab2a-805330751.tar.gz includes following files.

1. lab2_add.c
1) C program that implements and tests a shared variable add function,
   implements the specified command line options, and produces
   the specified output statistics.

2. SortedList.h
1) Header file describing the interfaces for linked list operations.

3. SortedList.c
1) C module that implements insert, delete, lookup, and length methods for
   a sorted doubly linked list.

4. lab2_list.c
1) C program that implements the specified command line optons and produces
   the specified output statistics.

5. lab2_add.csv
1) Containing all of results for all of the Part-1 tests.

6. lab2_list.csv
1) Containing all of results for all of the Part-2 tests.

7. Makefile
Compile the following targets

1) build: Compile the program lab2_add and lab2_list with the -Wall and
          -Wextra options.

2) tests: run all (over 200) specified test cases to generate results in
          CSV files.

3) graphs: Using gnuplot(1) and the supplied data reduction scripts to generate
           the required graphs.

4) dist: Create the deliverable tarball.

5) clean: Delete all programs and output created by the Makefile.

8. lab2_add.gp
1) Generating data reduction graphs for the multi-threaded add project.

9. lab2_list.gp
1) Generating data reduction graphs for the multi-threaded list project.

10. lab2_add-1.png
1) Threads and iterations required to generate a failure (with and without yields)

11. lab2_add-2.png
1) Average time per operation with and without yields

12. lab2_add-3.png
1) Average time per (single threaded) operation vs. the number of iterations.

13. lab2_add-4.png
1) Threads and iterations that can run successfully with yields under each of
   the Synchronization options.

14. lab2_add-5.png
1) Average time per (protected) operation vs the number of threads.

15. lab2_list-1.png
1) Average time per (single threaded) unprotected operation vs number of iterations
   (illustrating the correction of the per-operation cost for the list length).

16. lab2_list-2.png
1) Threads and iterations required to generate a failure (with and without yields).

17. lab2_list-3.png
1) Iterations that can run (protected) without failure.

18. lab2_list-4.png
1) Cost per operation vs the number of threads for the various Synchronization
   options.


Cited sources
https://ccle.ucla.edu/pluginfile.php/3439579/mod_resource/content/0/Week%205.pdf
https://ccle.ucla.edu/pluginfile.php/3440818/mod_resource/content/0/Week5.pdf


Questions
1) Questions 2.1.1 - causing conflicts:
(1) Why does it take many iterations before errors are seen?
=> The reason why it takes many iterations before errors are seen 
   because one thread does not interrupt other threads in execution
   when the number of iteration is low. In other words, the execution of
   previous thread is much faster than creating next thread,
   so race condition does not happen, and it can take many iterations
   before errors are seen.

(2) Why does a significantly smaller number of iterations so seldom fail?
=> As I mentioned above, the previous thread can finish executing the entire
   function more quickly before creating the next thread when the number of
   iteration is small. That's why significantly smaller number of 
   iterations so seldom fail.


2) Question 2.1.2 - cost of yielding:
(1) Why are the --yield runs so much slower?
=> The reason why the --yield runs so much a slower because it stops
   executing thread and yield CPU to new thread, so it takes time
   while interrupting and switching.

(2) Where is the additional time going?
=> As I mentioned above, the additional time is switching context.

(3) Is it possible to get valid per-operation timings if we are using the
    --yield option? If so, explain how. If not, explain why not.
=> It is impossible to get valid per-operation timings if we are using the
   --yield option because we cannot predict how much time it takes to switch
   context.


3) Question 2.1.3 - measurement errors:
(1) Why does the average cost per operation drop with increasing iterations?
=> The reason why the average cost per operation drop with increasing iteration
   because creating a new thread is much more expensive than iteration.

(2) If the cost per iteration is a function of the number of iterations, how
    do we know how many iterations to run (or what the "correct" cost is)?
=> If the number of iteration is increased, it will get close to 
   the correct cost. In other words, the cost will not change a lot 
   and reach a stable level, and we can get correct cost by calculating 
   the average cost in stable level.


4) Question 2.1.4 - cost of serialization:
(1) Why do all of the options perform similarly for low numbers of threads?
=> The reason why all of the options perform similarly for a low number of 
   threads because it is less likely to happen the race condition when the
   number of threads is low. 

(2) Why do all the three protected operations slow down as the number of
    threads rises?
=> If the number of threads increases, each thread has to wait for releasing 
   locks instead of doing operation, so it takes much time for waiting 
   for releasing locks.


5) Question 2.2.1 - Scalability of Mutex
(1) Compare the variation in time per mutex-protected operation vs the number
    of threads in Part-1 (adds) and Part-2 (sorted list).
    Comment on the general shapes of the curves, and explain 
    why they have this shape.
=> Both Part-1 and Part-2 have the linear lines, but the slope 
   of the linear line in Part-1 became low compared to the slope of the
   linear line in Part-2. The reason why they have linear line 
   because the more the number of threads increases, 
   the more it takes time to wait for other threads to release locks.
   Therefore, the cost per operation is proportional to the number of threads.

(2) Comment on the relative rates of increases and differences in the shapes
    of the curves, and offer an exlplanation for these differences.
=> The reason why the add's cost of operation is higher than the sorted list's
   cost of operation because it is more expensive searching in sorted list
   compared to adding and subtracting, so each thread for sorted list
   should wait a much longer time for releasing lock compared to adds.


6) Question 2.2.2 - Scalability of spin locks
(1) Compare the variation in time per protected operation vs the number of 
    threads for list operations protected by Mutex vs Spin locks.
    Comment on the general shapes of the curves, and explain why they have
    this shape.
=> For the low number of threads, the cost of operation for mutex is much more 
   expensive compared to the cost of operation for spin locks. In contrast,
   for the high number of threads, the cost of operation for spin locks is 
   much more expensive compared to the cost of operation for mutex.
   Moreover, both mutex and spin locks have the linear lines because
   the more the number of threads increases, the more it takes time to wait
   for other threads to release locks. Therefore, the cost per operation is
   proportional to the number of threads.

(2) Comment on the relative rates of increase and differences in the shapes
    of the curves, and offer an explanation for these differences.
=> In the lab2_list-4.png, the spin locks is much more increase than mutex.
   The reason is that the threads in spin locks check whether
   the lock is released or not without resting, so it uses much
   CPU times. In contrast, the threads in mutex just sleep when the locks
   are not released, and wake up when the locks are released, so it does not
   waste CPU times while waiting for releasing locks.
