#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <ctime>
#include <climits>
#include <map>
#include <queue>
#include "Sort_p.hpp"
#include "locks_bar.hpp"

#define NUM_BUCKETS 3

using namespace std;

// barrier<> *bar;
// mutex* lk;
// mutex* queue_lk;
// queue<int> sort_queue;

locks* lck;
barriers* bar;

vector<int> mins;
vector<thread*> threads;
vector<int> bar_req;
map<int, int> bucket_map[NUM_BUCKETS];

size_t NUM_THREADS;
struct timespec startTime, endTime;

/**
 * @brief   global init funciton for bucket sort
 * @return  None
 */
void global_init_bucket_bar(int k, int bar_type){
	lck = new locks(TAS, NUM_THREADS);
	bar = new barriers(bar_type, NUM_THREADS);
	
	for(int i=0; i<NUM_THREADS; i++){
		bar_req.push_back(true);
	}
	
	int a = INT_MAX/k;

	// Determine the min value to store in each bucket with INT_MAX being the max number storable in the last bucket
	for(int i=0; i<k; i++){
		mins.push_back(i*a);
	}
	mins.push_back(INT_MAX);
}

void global_init_bucket_lock(int k, int lock_type){
    lck = new locks(lock_type, NUM_THREADS);
	
	int a = INT_MAX/k;

	// Determine the min value to store in each bucket with INT_MAX being the max number storable in the last bucket
	for(int i=0; i<k; i++){
		mins.push_back(i*a);
	}
	mins.push_back(INT_MAX);
}

/**
 * @brief   global clean up funciton for bucket sort
 * @return  None
 */
void global_cleanup_bucket(){
	delete lck;
	delete bar;
}

/**
 * @brief   Function to printout the function runtime
 * @return  None
 */
void print_time(){
	unsigned long long elapsed_ns;
	elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
	printf("%llu\n",elapsed_ns);
}

/**
 * @brief   Function to sorts the given array with bucketsort algorithm
 * 			Function modified to run in multiple threads.
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
void* lkbucket_p_lck(vector<int> &a, int start, int end, int tid){
	
	// Go through the entire array, determine which bucket to insert the current element and insert it
	for(int i=start; i<=end; i++){
		for(int j=0; j<=NUM_BUCKETS; j++){
			if(a[i]<mins[j]){
				lck->lock();
				bucket_map[j-1].insert(pair<int, int>(a[i], 0));
				lck->unlock();
				break;
			}
		}
	}

	return 0;
}

int done_count = 0;

void* lkbucket_p_bar(vector<int> &a, int start, int end, int tid){

	bar->arrive_wait();

	// Go through the entire array, determine which bucket to insert the current element and insert it
	for(int i=start; i<=end; i++){
		for(int j=0; j<=NUM_BUCKETS; j++){
			if(a[i]<mins[j]){
				lck->lock();
				bucket_map[j-1].insert(pair<int, int>(a[i], 0));
				lck->unlock();
				break;
			}
		}
	}

	return 0;
}

/**
 * @brief   bucket sort function that sorts the array with parallel threads with bucketsort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int lkbucket_lock(vector<int> &a, int start, int end, int thread_num, int type){
	
	
	clock_gettime(CLOCK_MONOTONIC,&startTime);

	NUM_THREADS = thread_num;

	global_init_bucket_lock(NUM_BUCKETS, type);
	
	// Split the main array into subarrays depending on number of threads
	int size = (end+1)/NUM_THREADS, x=0;
	vector<int> arr_end;

	for(int i=1; i<=NUM_THREADS; i++){
		x = start+(i*size)-1;
		arr_end.push_back(x);
	}
	
	// start all threads to sort their respective sub-arrays using bucket sort algorithm
	int ret; size_t i;
	threads.resize(NUM_THREADS);

	for(i=1; i<NUM_THREADS; i++){
		int l_start = (arr_end[i-1]+1);
		int l_end = arr_end[i];
		if((end - l_end)<=size)
			l_end = end;
		threads[i] = new thread(lkbucket_p_lck,std::ref(a), l_start, l_end, i);
	}

	i = 0;
	lkbucket_p_lck(a, start, arr_end[i], i); // master also calls thread_main
	
	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		threads[i]->join();
		delete threads[i];
	}
	global_cleanup_bucket();

	// merge the sorted buckets into the main array
	int arr_i=start;
	for(int i=0; i<NUM_BUCKETS; i++){
		for (const auto& pair : bucket_map[i]) {
			a[arr_i++] = pair.first;
		}
	}
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	print_time();

	return 1;
}

/**
 * @brief   bucket sort function that sorts the array with parallel threads with bucketsort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int lkbucket_bar(vector<int> &a, int start, int end, int thread_num, int type){
	
	
	clock_gettime(CLOCK_MONOTONIC,&startTime);

	NUM_THREADS = thread_num;

	global_init_bucket_bar(NUM_BUCKETS, type);
	
	// Split the main array into subarrays depending on number of threads
	int size = (end+1)/NUM_THREADS, x=0;
	vector<int> arr_end;

	for(int i=1; i<=NUM_THREADS; i++){
		x = start+(i*size)-1;
		arr_end.push_back(x);
	}
	
	// start all threads to sort their respective sub-arrays using bucket sort algorithm
	int ret; size_t i;
	threads.resize(NUM_THREADS);

	for(i=1; i<NUM_THREADS; i++){
		int l_start = (arr_end[i-1]+1);
		int l_end = arr_end[i];
		if((end - l_end)<=size)
			l_end = end;
		threads[i] = new thread(lkbucket_p_bar,std::ref(a), l_start, l_end, i);
	}

	i = 0;
	lkbucket_p_bar(a, start, arr_end[i], i); // master also calls thread_main
	
	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		threads[i]->join();
		delete threads[i];
	}
	global_cleanup_bucket();

	// merge the sorted buckets into the main array
	int arr_i=start;
	for(int i=0; i<NUM_BUCKETS; i++){
		for (const auto& pair : bucket_map[i]) {
			a[arr_i++] = pair.first;
		}
	}
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	print_time();

	return 1;
}