#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include "Sort_openMP.hpp"

using namespace std;

struct timespec startTime, endTime;

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

    // Insert all remaining elements in the Left & Right array into the main array

	while(L_i<L_num){
        a[A_i++] = L_arr[L_i++];
    }

    while(R_i<R_num){
        a[A_i++] = R_arr[R_i++];
    }

	return 1;
}

/**
 * @brief   Merge sort function implementation to run on single thread
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
 * @brief   Merge sort function implementation to run on multiple threads using
 *          openMP 
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort_openMP(vector<int> &a, int start, int end, int thread_num){
    clock_gettime(CLOCK_MONOTONIC,&startTime);

	omp_set_num_threads(thread_num);

    // Check for exit condition
	if(start>=end)
		return 0;
		
    // Divide the array into 2 halfs, get both halfs sorted using seperate
    // threads and merge them together in a single thread.
	int mid = start + ((end-start)/2);
	
	#pragma omp parallel sections
	{        
		#pragma omp section
		merge_sort(a, start, mid);
		
		#pragma omp section
		merge_sort(a, mid+1, end);
	}

	merge(a, start, mid, end);
 
    clock_gettime(CLOCK_MONOTONIC,&endTime);
    print_time();

	return 1;
}

int merge_sort_openMP2(vector<int> &a, int start, int end, int thread_num, bool timelog){
    if(timelog)
        clock_gettime(CLOCK_MONOTONIC,&startTime);
    omp_set_num_threads(thread_num);
    // Check for exit condition
	if(start>=end)
		return 0;
		
    // Divide the array into 2 halfs, get both halfs sorted using seperate
    // threads and merge them together in a single thread.
	int mid = start + ((end-start)/2);
	
	#pragma omp parallel sections
	{        

		#pragma omp section
        {
    	omp_set_num_threads(thread_num);
		merge_sort_openMP2(a, start, mid, thread_num, false);
        }
		
		#pragma omp section
        {
        omp_set_num_threads(thread_num);
		merge_sort_openMP2(a, mid+1, end, thread_num, false);
        }
	}

	merge(a, start, mid, end);
 
    if(timelog){
        clock_gettime(CLOCK_MONOTONIC,&endTime);
        print_time();
    }

	return 1;
}

/**
 * @brief   Quick sort function implementation to run on single thread
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort(vector<int> &a, int start, int end){

    // check for exit condition
    if(end<=start)
        return 0;

    // Select the last element in the array as the pivot, and 
    int pid = end;
    int i=start-1, temp=0;
    
    // Iterate through the array from the beginning. If the element at the
    // current position is smaller than the pivot element, increment i, swap
    // current element with element at pos i and go to next element in the array
    for(int j=start; j<end; j++){
        if(a[j] < a[pid]){
            i++;
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            continue;
        }
    }
    
    // At the end, increment i and swap the element at pos pivot with element
    // at pos i
    i++;
    temp = a[pid];
    a[pid] = a[i];
    a[i] = temp;

    // sort the array to the left and right of the pivoted element
	quick_sort(a, start, i-1);
	quick_sort(a, i+1,end);
	
	return 1;
}

/**
 * @brief   Quick sort function implementation to run on multiple threads using
 *          openMP 
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort_openMP(vector<int> &a, int start, int end, int thread_num){

	
	clock_gettime(CLOCK_MONOTONIC,&startTime);

	omp_set_num_threads(thread_num);

    // check for exit condition
    if(end<=start)
        return 0;

    // Select the last element in the array as the pivot, and 
    int pid = end;
    int i=start-1, temp=0;
    
    // Iterate through the array from the beginning. If the element at the
    // current position is smaller than the pivot element, increment i, swap
    // current element with element at pos i and go to next element in the array
    for(int j=start; j<end; j++){
        if(a[j] < a[pid]){
            i++;
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            continue;
        }
    }
    
    // At the end, increment i and swap the element at pos pivot with element
    // at pos i
    i++;
    temp = a[pid];
    a[pid] = a[i];
    a[i] = temp;

    // sort the array to the left and right of the pivoted element in seperate
    // threads

	#pragma omp parallel sections
	{

		#pragma omp section
    	quick_sort(a, start, i-1);

		#pragma omp section
    	quick_sort(a, i+1,end);
	}
	
	clock_gettime(CLOCK_MONOTONIC,&endTime);
	print_time();

	return 1;
}