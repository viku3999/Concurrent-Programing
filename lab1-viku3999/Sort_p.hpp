#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;


/**
 * @brief   forkjoin sort function that sorts the array with parallel threads with mergesort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int forkjoin(vector<int> &a, int start, int end, int threads);

/**
 * @brief   bucket sort function that sorts the array with parallel threads with bucketsort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int lkbucket(vector<int> &a, int start, int end, int threads);

int lkbucket_queue(vector<int> &a, int start, int end, int thread_num);