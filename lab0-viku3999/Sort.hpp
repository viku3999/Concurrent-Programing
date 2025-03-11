#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief   merge sort function that divides the given array of integers into 2 and sorts them by merging
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort(vector<int> &a, int start, int end);

/**
 * @brief   quick sort function implementation
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort(vector<int> &a, int start, int end);