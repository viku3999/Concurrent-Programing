#include "Sort.hpp"

using namespace std;

/**
 * @brief   Merge function that merges the two halfs of an array of integers in ascending order.
 * @return  None
 */
void merge(vector<int> &a, int L, int mid, int R){

    // Get the number of integers in both halfs of the array and make a local copy of them
    int L_num = mid - L + 1;
    int R_num = R - mid;
    vector<int> L_arr, R_arr;

    for(int i=0; i<L_num; i++){
        L_arr.push_back(a[L+i]);
    }
    
    for(int i=0; i<R_num; i++){
        R_arr.push_back(a[mid+1+i]);
    }

    // iterate through the Left and Right copies of the main array and compare
    // each element. The smaller element shall be inserted into the main array 
    // location. 
    int L_i=0, R_i=0, a_i=L;

    while((L_i<L_num)&&(R_i<R_num)){
        if(L_arr[L_i]<R_arr[R_i]){
            a[a_i] = L_arr[L_i];
            L_i++;
            a_i++;
        }
        else if(R_arr[R_i]<L_arr[L_i]){
            a[a_i] = R_arr[R_i];
            R_i++;
            a_i++;
        }
        else{
            a[a_i] = R_arr[R_i];
            R_i++;
            a_i++;
            a[a_i] = L_arr[L_i];
            L_i++;
            a_i++;
        }
    }

    // Insert all remaining elements in the Left / Right array into the main array

    while(L_i<L_num){
        a[a_i] = L_arr[L_i];
        L_i++;
        a_i++;
    }

    while(R_i<R_num){
        a[a_i] = R_arr[R_i];
        R_i++;
        a_i++;
    }

}

/**
 * @brief   merge sort function that divides the given array of integers into 2 and sorts them by merging
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort(vector<int> &a, int start, int end){

    // Check for exit condition
    if(start>=end){
        return 0;
    }

    // Divide the array into 2 halfs, get both halfs sorted and merge them 
    // together 
    int mid = start + (end - start) / 2;

    merge_sort(a, start, mid);
    merge_sort(a, mid+1, end);
    merge(a, start, mid, end);

	return 1;
}

/**
 * @brief   quick sort function implementation
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort(vector<int> &a, int start, int end){

    // check for exit condition
    if(end<=start)
        return 0;

    // Select the last element in the array as the pivot, and 
    int pid = end-1;
    int i=start-1, temp=0;
    
    // Iterate through the array from the beginning. If the element at the
    // current position is smaller than the pivot element, increment i, swap
    // current element with element at pos i and go to next element in the array
    for(int j=start; j<end-1; j++){
        if(a[j] < a[pid]){
            i++;
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            continue;
        }
    }
    
    // At the end, increment i and swap the element at pos pivot with element at pos i
    i++;
    temp = a[pid];
    a[pid] = a[i];
    a[i] = temp;

    // sort the array to the left and right of the pivoted element
    quick_sort(a, start, i);
    quick_sort(a, i+1,end);
	return 1;
}