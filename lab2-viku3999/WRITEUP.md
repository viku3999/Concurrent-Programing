# Lab 1 Write-Up
## Description
For this lab, I have implemented various locks and barriers primitives learnt in this class. using Sequential and Release consistencies. I have implemented the following primitives: 
- Test and Set Lock (TAS)
    - Implemented in Seq and Release Consistencies
    - A simple primitive where all threads try to set an common atomic flag and get the lock if they succeed.
    - Although simple, can lead to high cpu usage due to increased contention to acquire flag
- Test and Test and Set Lock (TTAS)
    - Implemented in Seq and Release Consistencies
    - Build on TAS and reduces contention by making threads to poll the status of the flag before acquiring it.
- Ticket Lock
    - Implemented in Seq Consistency
    - Ensures all threads get fair and equal opportuinity to acquire the lock by introdicing an classic ticketing system to ensure FIFO system.
- Peterson
    - Implemented in Seq and Release Consistencies for only 2 threads
    - Provides equal opportunity for each thread to acquire the lock nad provides mutual exclusion.
- Mellor-Crummey Scott lock (MCS)
    - Implemented in Seq Consistencies
    - A singly linked queue based lock to promote FIFO system that allows the waiting threads to complete other tasks till they acquire the lock (this feature is not implemented in this program. All threads will be busy-waiting).
- Sense Reverse Barrier
    - Implemented in Seq and Release Consistencies
    - Barrier primitive that ensures all threads arrive by maintaining a count of all arrived threads.
- pthread lock and barrier
    - Default c++ implementation

## Code Organization
The code is written in C++. The implementation for primitives are in the file `locks_bar.cpp` which allows for easy portability. Counter implementation is in `counters.cpp` which shall handle the command line interface, file handeling, and shall use the lock/barrier primitives defined in `lock_bar.cpp` file. Implementation for the bucket sort algorithm is same as previous lab, `main.cpp` file holds  the command line interface implementation as well as takes the input from an input file and provides an output in the specified file name.Bucketsort algorithm implementation is within the `Sort_p.cpp` file and shall use the lock/barrier primitives defined in `lock_bar.cpp` file.

---

## Files Submitted and Description

### counter.cpp
- Contains the main function that implements the counter logic.
- Implements the following logic:
	- Parsing through command line arguments, validating them, and saving the required info.
	- Printing error messages if the programs recieves any inncorrect input from the command line.
	- Performing the counting operation using the given lock/barrier primitive for given number of threads and number of iterations.
	- Final count across all threads is saved in the given output file name.

### locks_bar.cpp
- Contains the implementation for various locks and barrier primitives in a class structure and makes for easy portability
- Implements the following locks primitives 
    - TAS -> SEQ and REL consistencies
    - TTAS -> SEQ and REL consistencies
    - Ticket -> SEQ consistencies
    - Peterson -> SEQ and REL consistencies
    - MCS -> SEQ and REL consistencies
    - pthread -> SEQ consistencies
- Implements the following barrier primitives 
    - Sense reversal -> SEQ and REL consistencies
    - pthread -> c++ default implementation

### locks_bar.hpp
- Contains the class declarations for the lock/barrier primitives.

### main.cpp
- Contains the main function which implements the bucket sorting algorithm.
- Implements the following logic:
	- Parsing through command line arguments, validating them, and saving the required info.
	- Printing error messages if the programs recieves any inncorrect input from the command line.
	- Checks if the provided input file name is valid and returns error.
	- Integer data is extraced from the given input file and saved as an vector<int> array.
	- Calling the sorting algorithms with the approporiate lock/barrier primitive based on command line input.
	- Sorted data is saved in the given output file name.

### Sort_p.cpp
- Contains the bucketsort algorithm implementation in cpp that can utilize multiple threads to reduce runtime.
- Uses the lock/barrier primitives given by user.

### Sort_p.hpp
- Contains the function declarations for the bucketsort algorithm.

### Makefile
- Used to compile, link all files related to this project and create the executables `mysort` and `counter `.
- Also used to clean all object files and dependency files linked to the `mysort`, and `counter` executable if needed.

## Extant Bugs
In the current version, I am facing some issues with the MCS lock. When running the counter program, I am expecting a final count value of 1,000,000 but I am always falling short of 100,00-200,000 counts, and unable to fix this issue within the deadline.

## Challenges Faced:
I faced challenges mainly when implementing MCS lock and Petersons lock and defining the data structure for the queues took a bit more effort than previously estimated. For Petersons lock, my implementation for all other primitives did not require tid, and soI wanted peterson to behave the same way, which also took some extra effort.

---

## Performance Analysis

### Counter - Locks
Test conditions: 4 Threads, 250000 iterations
| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
|Test-and-set lock - sequential             |0.293850382 | 84.44 | 99.84 | 145 |
|Test-and-test-and-set lock  - sequential   |0.285516714 | 99.31 | 99.95 | 143 |
|Ticket lock  - sequential	                |0.530223035 | 99.79 | 99.94 | 144 |
|MCS Lock  - sequential	                    |0.127829608 | 83.46 | 99.78 | 145 |
|pthread  - sequential	                    |0.038927721 | 97.95 | 99.41 | 143 |
|peterson  - sequential	                    |0.092600847 | 95.66 | 98.48 | 140 |
|Test-and-set lock - release	            |0.255321589 | 84.1	 | 9.8	 | 144 |
|Test-and-test-and-set lock - release	    |0.134598151 | 98.6	 | 9.96	 | 145 |
|peterson - release	                        |0.308879119 | 99.14 | 99.71 | 144 |

### Counter - Barrier
Test conditions: 4 Threads, 250000 iterations
| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
|Sense reversal barrier - sequential	| 1.106413809	| 99.67	| 99.74	| 147
|pthread - barrier - sequential	        | 42.46136445	| 86.96	| 97.67	| 147
|Sense reversal barrier - release	    | 0.378765368	| 99.3	| 99.31	| 150


### Sorting - Locks
Test conditions: 4threads, sort file-550000in1-1000000skew.txt (given in autograder_tests folder), Bucketsort algorithm

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
|Test-and-set lock - sequential	            |0.838371039	|88.75	|98.72	|8,004
|Test-and-test-and-set lock  - sequential	|0.59258923	    |98.66	|99.81	|8,004
|Ticket lock  - sequential	                |0.754044909	|99.53	|99.88	|8,005
|MCS Lock  - sequential	                    |0.369430032	|89.88	|98.62	|8,003
|pthread  - sequential	                    |1.044896835	|90.04	|98.49	|8,000
|peterson  - sequential	                    |1.023957016	|90.31	|98.57	|8,003
|Test-and-set lock - release	            |1.110427478	|89.84	|98.53	|8,003
|Test-and-test-and-set lock - release	    |1.171934975	|89.76	|98.48	|8,004
|peterson - release	                        |0.689041059	|98.18	|99.61	|8,003


### Sorting - Barrier
Test conditions: 4threads, sort file-550000in1-1000000skew.txt (given in autograder_tests folder), Bucketsort algorithm

| Primitive    | Runtime (s) | L1 cache hit (%) | Branch Pred Hit Rate (%) | Page Fault Count (#)|
|--------------|-------------|------------------|--------------------------|---------------------|
|Sense reversal barrier  - sequential	|1.252627191	|89.54	|98.83	|8,006|
|pthread - barrier - sequential	        |1.047887624	|89.73	|98.81	|8,001|

---

### Performance Analysis - Locks
By analysing the table, pthread-seq and ticket lock has the fastest and slowest execution respectively for the counter program and both of them have similar L1 cache hit % at 97.95% and 99.79% respectively. But in the sorting program, MCS lock is the fastest and TTAS-rel is the slowest, and also have similar L1 cache hit % at 89.88% and 89.76% respectively. From this, we can infer that in simpler tasks, pthread has better performance, but in heaver workloads, MCS is the better option, followed by TTAS-seq, and peterson-rel algorithm. Overall, we can infer that ticket-seq provides a good balance between runtime, L1 cache hit%, Branch Pred Hit Rate% and runtime. We can also see that Seq consistency generally fares better than release consistency.

### Performance Analysis - Barriers
From the table, we can see that Sense reversal barrier performes better in light workloads such as the counter program when compared to pthread which is very slow, and does not offer any improvement in L1 cache hit % or Branch Hit Rate% when compated to Sense reversal barrier. On the contary, in heavy workloads such as the sorting algorithm, pthread barrier performance is similar to sense reversal barrier. Hence, we can say that for lighter workloads, it is better to avoid pthread barrier and use sense reversal barrier.