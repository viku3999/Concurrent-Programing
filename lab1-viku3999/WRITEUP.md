# Parallelization Strategy
For this assignment, my approach for the two sorting algorithms are similar; split the array based on the number of threads available such that each thread will have roughly the same number of elements to sort. After each thread is done sorting, I would then merge them into a single sorted array.

# Performance comparison
While running the autograder, I found that bucket sort is faster than forkjoin for smaller array sizes. But bucket sort fails when it has to sort arrays of large sizes or skewed elements. In contrast forkjoin with merge sort implementation is fast and the run time also scales up with respective to the number of elements to sort.

# Code Organization
The `main.cpp` file holds the cpp implementation for this assignment. This file hold the command line interface implementation as well as takes the input from an input file and provides an output in the specified file name.  Both forkjoin algorithm and bucketsort algorithm implementation is within the `Sort_p.cpp` file. These algorithms are implemented to run on multiple threads. Since forkjoin is implemented with merge sort, each thread sorts the array recursively.

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

### Sort_p.cpp
- Contains the forkjoin algorithm implementation in cpp that can utilize multiple threads to reduce runtime. Merge sort algorithm is utilized to sort the array via recursive method.
- Contains the bucketsort algorithm implementation in cpp that can utilize multiple threads to reduce runtime.

### Sort_p.hpp
- Contains the function declarations for the bucketsort algorithm and forkjoin sorting algorithm.

### Makefile
- Used to compile, link all files related to this project and create the executable `mysort`.
- Also used to clean all object files and dependency files linked to the `mysort` executable if needed.

---

## Bugs
While doing this assignment, I did encounter multiple any bugs, most of which are somehow related to the 'off by one' error. One bug that is extant in this code is there is no upper limit to the number of threads that can utilized by the sorting algorithm. Hence, if the system is asked to execute the program using more then the number of available threads in the hardware, the program/system might crash. It is to be noted that I have not tested this case in any system.