# Ease of parallelization between pthreads and OpenMP
I personally found using OpenMP for parallelization to be easier than pthread since we did not have to handle any synchronisation primitives in the former while in the latter we had to declare, initialize and destroy any locks/barriers that we created using pthread method. Incase we are using barriers, We also had to ensure all threads reach all barriers until the last thread completes execution so we had to design our algrithm accordingly. But with OpenMP, we do not have to handle all these scenarios and so the algorithm can be a bit simpler.

# Parallelization Strategy
I used the mergesort and quicksort code from lab0 as a starting point for this lab. I was able to parallize the section of code that sorts 2 halfs of the array independently in both mergesort and quicksort algorithms by making them as seperate sections and used the `#pragma omp parallel sections` command to execute them in parallel. I had also declared the number of threads available to OpenMP at the start of the sorting algorithm functions.


# Performance comparison between OpenMP and pthread
The file `550000in1-1000000skew.txt` given in lab 1 autograder folder was used to compare the performance of the 2 parallelization methods. Comparison has done between `OpenMP` parallelization method with `mergesort` and `quicksort` algorithms from this lab and `pthread` parallelization method with `Forkjoin - mergesort` algorithm from lab1.

| Sorting Algorithm  | Num of Threads | Runtime (ms) | Runtime (ns) |
|--------------------|----------------|--------------|--------------|
|OpenMP - Mergesort	 |2				  |63.288769	|63288769	   |
|OpenMP - Quicksort	 |2				  |26.498964	|26498964	   |
|pthread - Forkjoin	 |2				  |102.627339	|102627339	   |
|OpenMP - Mergesort	 |4				  |62.114378	|62114378	   |
|OpenMP - Quicksort	 |4				  |26.496062	|26496062	   |
|pthread - Forkjoin	 |2				  |41.563357	|41563357	   |
|OpenMP - Mergesort	 |6				  |62.970999	|62970999	   |
|OpenMP - Quicksort	 |6				  |26.565119	|26565119	   |
|pthread - Forkjoin	 |2				  |34.689504	|34689504	   |

The runtime for OpenMP seems to be a bit consistent with increase in number of threads for both Mergesort and Quicksort at 63ms and 26ms respectively, with quicksort performing significantly faster then mergesort, but the runtime for Forkjoin-Mergesort algorithms decreases significantly with increase in number of threads as seen in the above table. 

We can infer that although it is easy to parallelize sequential code using OpenMP, there is a performance tradeoff. Hence, it is better pthread parallelization method if there is a need for significant performance improvement with increase in thread count.

# Code Organization
The `main.cpp` file holds the cpp implementation for this assignment. This file hold the command line interface implementation as well as takes the input from an input file and provides an output in the specified file name.  

Both mergesort algorithm and quicksort algorithm implementation using OpenMP is within the `Sort_openMP.cpp` file. These algorithms are implemented to run on multiple threads.

---

## Files

### main.cpp
- Contains the main function which implements this assigment.
- Implements the following logic:
	- Parsing through command line arguments, validating them, and saving the required info.
	- Printing error messages if the programs recieves any inncorrect input from the command line.
	- Checks if the provided input file name is valid and returns error.
	- Integer data is extraced from the given input file and saved as an vector<int> array.
	- Calling the approporiate sorting algorithms (Merge/Quick sort) based on command line input.
	- Sorted data is saved in the given output file name.

### Sort_openMP.cpp  
- Contains the Mergesort and Quicksort algorithm implementation using cpp. Utilized OpenMP to run the algorithm concurrently/sequentialy based on available H/W cores availability.

### Sort_openMP.hpp
- Contains the function declarations for the Mergesort and Quicksort algorithms.

### Makefile
- Used to compile, link all files related to this project and create the executable `mysort`.
- Also used to clean all object files and dependency files linked to the `mysort` executable if needed.

---

## Bugs
While doing this assignment, I did not encounter any bugs related to the sorting algorithm or parallizing the code. 