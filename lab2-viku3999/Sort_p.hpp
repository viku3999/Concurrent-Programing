#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief   bucket sort function that sorts the array with parallel threads with bucketsort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int lkbucket_lock(vector<int> &a, int start, int end, int thread_num, int type);

int lkbucket_bar(vector<int> &a, int start, int end, int thread_num, int type);