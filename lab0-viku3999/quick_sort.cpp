#include <fstream>
#include <vector>
#include "quick_sort.hpp"
#include "Sort.hpp"

using namespace std;
vector<int> sort_arr;

/**
 * @brief   quick sort function implementation
 * @return  None
 */
void quick_sort_rec(int start, int end){

    // check for exit condition
    if(end<=start)
        return;

    // Select the last element in the array as the pivot, and 
    int pid = end-1;
    int i=start-1, temp=0;
    
    // Iterate through the array from the beginning. If the element at the
    // current position is smaller than the pivot element, increment i, swap
    // current element with element at pos i and go to next element in the array
    for(int j=start; j<end-1; j++){
        if(sort_arr[j] < sort_arr[pid]){
            i++;
            temp = sort_arr[i];
            sort_arr[i] = sort_arr[j];
            sort_arr[j] = temp;
            continue;
        }
    }
    
    // At the end, increment i and swap the element at pos pivot with element at pos i
    i++;
    temp = sort_arr[pid];
    sort_arr[pid] = sort_arr[i];
    sort_arr[i] = temp;

    // sort the array to the left and right of the pivoted element
    quick_sort_rec(start, i);
    quick_sort_rec(i+1,end);
}

/**
 * @brief   Driver function to initialize the sorting process by extracting the
 *          numbers from the given input file and write the sorted numbers into
 *          the output file.
 * @return  1 -> if sort is successfull
 *          0 -> if sort is uncuccessfull
 */
int quick_sort(char* ip_file, char* op_file){
    
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
    string line;
    int x=0;

    while(getline(fin, line)){
        x = atoi(&line[0]);
        sort_arr.push_back(x);
    }

    // Sort the array using quick sort alg and save the results into the output file
    int gh = sort_arr.size();
    // quick_sort_rec(0, gh);
    quick_sort(sort_arr, 0, gh);
    for(int i=0; i<gh; i++){
        fout<<sort_arr[i]<<endl;       
    }
    fin.close();
    fout.close();
    return(1);
}