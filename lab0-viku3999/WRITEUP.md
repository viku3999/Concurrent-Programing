# Code Organization
The `main.cpp` file holds the cpp implementation for this assignment. The quick sort algorithm implementation is within the `quick_sort.cpp` file and similarly the merge sort algorithm implementation is in the `merge_sort.cpp` file. These algorithms are implemented recursively. Both sorting algorithms takes the input from an input file and provides an output in the specified file name.

---

## Files

### main.cpp
- Contains the main function which implements this assigment.
- Implements the following logic:
	- Parsing through command line arguments, validating them, and saving the required info.
	- Printing error messages if the programs recieves any inncorrect input from the command line.
	- Calling the approporiate sorting algorithms (Merge/Quick sort) based on command line input.

### merge_sort.cpp
- Contains the merge sort algorithm implementation in cpp done via recursive method.
- Checks if the provided input file name is valid and returns error.
- Integer data is extraced from the given input file and saved as an vector<int> array.
- Sorted data is saved in the given output file name.

### quick_sort.cpp
- Contains the quick sort algorithm implementation in cpp done via recursive method.
- Checks if the provided input file name is valid and returns error.
- Integer data is extraced from the given input file and saved as an vector<int> array.
- Sorted data is saved in the given output file name.

### merge_sort.hpp
- Contains the function declarations for the merge sort algorithm.

### quick_sort.hpp
- Contains the function declarations for the quick sort algorithm.

### Makefile
- Used to compile, link all files related to this project and create the executable `mysort`.
- Also used to clean all object files and dependency files linked to the `mysort` executable if needed.

---

## Bugs
While doing this assignment, I did not encounter any bugs related to the sorting algorithm. But while testing out the command line arguments, I was able to find some edge cases such as the input file format is incorrect, file name is not given, sorting algorithm is not given. So I have handeled these cases in the `main.cpp` file and provides an feedback to the user.