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

#define NUM_BUCKETS 3

using namespace std;

barrier<> *bar;
mutex* lk;
mutex* queue_lk;
queue<int> sort_queue;

vector<int> mins;
vector<thread*> threads;
map<int, int> bucket_map[NUM_BUCKETS];

size_t NUM_THREADS;
struct timespec startTime, endTime;

/**
 * @brief   global init up funciton for forkjoin sort
 * @return  None
 */
void global_init(){
	bar = new barrier(NUM_THREADS);
}

/**
 * @brief   global clean up funciton for forkjoin sort
 * @return  None
 */
void global_cleanup(){
	delete bar;
}

/**
 * @brief   global init funciton for bucket sort
 * @return  None
 */
void global_init_bucket(int k){
	lk = new mutex();
	queue_lk = new mutex();
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
	delete lk;
	delete queue_lk;
}

/**
 * @brief   Function to printout the function runtime
 * @return  None
 */
void print_time(){
	unsigned long long elapsed_ns;
	elapsed_ns = (endTime.tv_sec-startTime.tv_sec)*1000000000 + (endTime.tv_nsec-startTime.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);
}

/**
 * @brief   Merge function that merges the two halfs of an array of integers in ascending order.
 * 			Function to run in only 1 thread
 * @return  None
 */
int merge(vector<int> &a, int start, int mid, int end){
	
	// Get the number of integers in both halfs of the array and make a local copy of them
    int L_num = mid - start + 1;
    int R_num = end - mid;
    vector<int> L_arr, R_arr;
	int L_i=0, R_i=0, A_i=start;

    for(int i=0; i<L_num; i++){
        L_arr.push_back(a[start+i]);
    }
    
    for(int i=0; i<R_num; i++){
        R_arr.push_back(a[mid+1+i]);
    }

	
    // iterate through the Left and Right copies of the main array and compare
    // each element. The smaller element shall be inserted into the main array 
    // location. 

	while((L_i<L_num)&&(R_i<R_num)){
		if(L_arr[L_i]<R_arr[R_i]){
			a[A_i++] = L_arr[L_i++];
		}
		else if(R_arr[R_i]<L_arr[L_i]){
			a[A_i++] = R_arr[R_i++];
		}
		else{
			a[A_i++] = R_arr[R_i++];
			a[A_i++] = L_arr[L_i++];
		}
	}

    // Insert all remaining elements in the Left / Right array into the main array

	while(L_i<L_num){
        a[A_i++] = L_arr[L_i++];
    }

    while(R_i<R_num){
        a[A_i++] = R_arr[R_i++];
    }

	return 1;
}

/**
 * @brief   merge sort function that divides the given array of integers into 2 and sorts them by merging
 * 			Function to run in only 1 thread
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort(vector<int> &a, int start, int end){
	
    // Check for exit condition
	if(start>=end)
		return 0;
		
    // Divide the array into 2 halfs, get both halfs sorted and merge them 
    // together 
	int mid = start + ((end-start)/2);

	merge_sort(a, start, mid);
	merge_sort(a, mid+1, end);
	merge(a, start, mid, end);

	return 1;
}

/**
 * @brief   merge sort function that divides the given array of integers into 2 and sorts them by merging. 
 * 			Function modified to run in multiple threads.
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
void* merge_sort_p(vector<int> &a, int start, int end, int tid){
	
	// Check for exit condition
	if(start>=end)
		return 0;
	bar->arrive_and_wait();


    // Divide the array into 2 halfs, get both halfs sorted and merge them together 
	int mid = start + ((end-start)/2);
	bar->arrive_and_wait();

	merge_sort(a, start, mid);
	merge_sort(a, mid+1, end);
	merge(a, start, mid, end);
	bar->arrive_and_wait();

	return 0;
}

/**
 * @brief   forkjoin sort function that sorts the array with parallel threads with mergesort algorithm
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int forkjoin(vector<int> &a, int start, int end, int thread_num){
	clock_gettime(CLOCK_MONOTONIC,&startTime);
	
	NUM_THREADS = thread_num;
	global_init();
	
	int ret; size_t i;
	threads.resize(NUM_THREADS);

	// Split the main array into subarrays depending on number of threads
	int size = (end+1)/NUM_THREADS, x=0;
	vector<int> arr_end;

	for(int i=1; i<=NUM_THREADS; i++){
		x = start+(i*size)-1;
		arr_end.push_back(x);
	}

	// start all threads to sort their respective sub-arrays using merge-sort algorithm
	for(i=1; i<NUM_THREADS; i++){
		
		int l_start = (arr_end[i-1]+1);
		int l_end = arr_end[i];
		
		if(((end - l_end)<=size) && (i+1 == NUM_THREADS)){
			l_end = end;
			arr_end[i] = end;
		}

		threads[i] = new thread(merge_sort_p,std::ref(a), l_start, l_end, i);
	}

	i = 0;
	merge_sort_p(a, start, arr_end[i], i); // master also calls thread_main
	
	// join threads
	for(size_t i=1; i<NUM_THREADS; i++){
		threads[i]->join();
		delete threads[i];
	}
	global_cleanup();

	// Merge the sorted sub-arrays into one
	for(int i=1; i<NUM_THREADS; i++){
		merge(a, start, arr_end[i-1], arr_end[i]);
	}
	
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	print_time();

	return 1;
}

/**
 * @brief   Function to sorts the given array with bucketsort algorithm
 * 			Function modified to run in multiple threads.
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
void* lkbucket_p(vector<int> &a, int start, int end, int tid){

	// Go through the entire array, determine which bucket to insert the current element and insert it
	for(int i=start; i<=end; i++){
		for(int j=0; j<=NUM_BUCKETS; j++){
			if(a[i]<mins[j]){
				lk->lock();
				bucket_map[j-1].insert(pair<int, int>(a[i], 0));
				lk->unlock();
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
int lkbucket(vector<int> &a, int start, int end, int thread_num){
	clock_gettime(CLOCK_MONOTONIC,&startTime);

	NUM_THREADS = thread_num;
	global_init_bucket(NUM_BUCKETS);

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
		threads[i] = new thread(lkbucket_p,std::ref(a), l_start, l_end, i);
	}
	i = 0;
	lkbucket_p(a, start, arr_end[i], i); // master also calls thread_main
	
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

int get_element(){

	if(sort_queue.empty()){
		return -1;
	}

	queue_lk->lock();
	int temp = sort_queue.front();
	sort_queue.pop();
	queue_lk->unlock();
	return temp;
}

void print_queue(){
	queue<int> q_temp = sort_queue;

	while(!q_temp.empty()){
		printf("%d ", q_temp.front());
		q_temp.pop();
	}

    cout << '\n';
}

void* lkbucket_queue_p(int tid){
	// should pop an element from the queue
	// insert the element into the respective bucket
	// If queue is empty, wait until an element is inserted?

	// Go through the entire array, determine which bucket to insert the current element and insert it
	int temp = get_element();
	while(temp != -1){
		for(int j=0; j<=NUM_BUCKETS; j++){
			if(temp<mins[j]){
				lk->lock();
				bucket_map[j-1].insert(pair<int, int>(temp, 0));
				lk->unlock();
				break;
			}
		}
		temp = get_element();
	}
	// cout<<"Done\n";

	return 0;
}

void lkbucket_queue_push(vector<int> &a, int start, int end){
	// pushing num to queue
	for(int i=start; i<=end; i++){
		sort_queue.push(a[i]);
	}
}

int lkbucket_queue(vector<int> &a, int start, int end, int thread_num){
	clock_gettime(CLOCK_MONOTONIC,&startTime);

	lkbucket_queue_push(a, start, end);

	NUM_THREADS = thread_num;
	global_init_bucket(NUM_BUCKETS);
	threads.resize(NUM_THREADS);

	for(int i=1; i<NUM_THREADS; i++){
		threads[i] = new thread(lkbucket_queue_p, i);
	}

	lkbucket_queue_p(0);

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