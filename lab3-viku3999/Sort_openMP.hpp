#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

/**
 * @brief   Merge sort function implementation to run on multiple threads using
 *          openMP 
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort_openMP(vector<int> &a, int start, int end, int thread_num);

/**
 * @brief   Quick sort function implementation to run on multiple threads using
 *          openMP 
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort_openMP(vector<int> &a, int start, int end, int thread_num);

int merge_sort_openMP2(vector<int> &a, int start, int end, int thread_num, bool timelog);