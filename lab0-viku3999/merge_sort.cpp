#include <fstream>
#include <vector>
#include "merge_sort.hpp"
#include "Sort.hpp"
using namespace std;

// /**
//  * @brief   Merge function that merges the two halfs of an array of integers in ascending order.
//  * @return  None
//  */
// void merge(vector<int> &a, int L, int mid, int R){

//     // Get the number of integers in both halfs of the array and make a local copy of them
//     int L_num = mid - L + 1;
//     int R_num = R - mid;
//     vector<int> L_arr, R_arr;

//     for(int i=0; i<L_num; i++){
//         L_arr.push_back(a[L+i]);
//     }
    
//     for(int i=0; i<R_num; i++){
//         R_arr.push_back(a[mid+1+i]);
//     }

//     // iterate through the Left and Right copies of the main array and compare
//     // each element. The smaller element shall be inserted into the main array 
//     // location. 
//     int L_i=0, R_i=0, a_i=L;

//     while((L_i<L_num)&&(R_i<R_num)){
//         if(L_arr[L_i]<R_arr[R_i]){
//             a[a_i] = L_arr[L_i];
//             L_i++;
//             a_i++;
//         }
//         else if(R_arr[R_i]<L_arr[L_i]){
//             a[a_i] = R_arr[R_i];
//             R_i++;
//             a_i++;
//         }
//         else{
//             a[a_i] = R_arr[R_i];
//             R_i++;
//             a_i++;
//             a[a_i] = L_arr[L_i];
//             L_i++;
//             a_i++;
//         }
//     }

//     // Insert all remaining elements in the Left / Right array into the main array

//     while(L_i<L_num){
//         a[a_i] = L_arr[L_i];
//         L_i++;
//         a_i++;
//     }

//     while(R_i<R_num){
//         a[a_i] = R_arr[R_i];
//         R_i++;
//         a_i++;
//     }

// }

// /**
//  * @brief   merge sort function that divides the given array of integers into 2 and sorts them by merging
//  * @return  None
//  */
// void merge_sort_rec(vector<int> &a, int start, int end){

//     // Check for exit condition
//     if(start>=end){
//         return;
//     }

//     // Divide the array into 2 halfs, get both halfs sorted and merge them 
//     // together 
//     int mid = start + (end - start) / 2;

//     merge_sort_rec(a, start, mid);
//     merge_sort_rec(a, mid+1, end);
//     merge(a, start, mid, end);
// }

/**
 * @brief   Driver function to initialize the sorting process by extracting the
 *          numbers from the given input file and write the sorted numbers into
 *          the output file.
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int merge_sort(char* ip_file, char* op_file){

    // Try to open the given input and output files and return 0 if we are 
    // unable to open the files
    ifstream fin;
    fin.open(ip_file);
    if(!fin)
        return 0;

    ofstream fout;
    fout.open(op_file);
    if(!fout)
        return 0;

    // Read each line of the input file, convert the number from char to int 
    // and save it to a vector array 
    vector<int> sort_arr;
    string line;
    int x=0;
    
    while(getline(fin, line)){
        x = atoi(&line[0]);
        sort_arr.push_back(x);
    }

    // Sort the array using merge sort alg and save the results into the output file
    int gh = sort_arr.size();
    merge_sort(sort_arr, 0, gh-1);

    for(int i=0; i<gh; i++){
        fout<<sort_arr[i]<<endl;       
    }
    fin.close();
    fout.close();

    return 1;
}