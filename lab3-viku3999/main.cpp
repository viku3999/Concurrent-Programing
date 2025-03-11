#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Sort_openMP.hpp"

using namespace std;

/**
 * @brief   Main function to carry out the program requiremnts.
 * @return  1 -> if program is successfull
 *          0 -> if program is uncuccessfull
 */
int main(int argc, char *argv[]){

    char* ip_file=0;
    char* op_file=0;  
    int i_flag=0, o_flag=0, t_flag=0, alg_flag=0;

    /**
     * Iterate through the command line arguments
     * 
     * If we recieve '--name', print out your name
     * 
     * If we recieve '-i' cmd, do the following checks:
     *  - Command is repeated multiple times and return error if so
     *  - A file name is given as the immediate next input and return error if not
     *  - Input file format is '.txt'
     * If all the above cases pass, save the input file name.
     *  
     * If we recieve '-o' cmd, do the following checks:
     *  - Command is repeated multiple times and return error if so
     *  - A file name is given as the immediate next input and return error if not
     * If all the above cases pass, save the output file name.
     * 
     * If we recieve '--alg' cmd, do the following checks:
     *  - Command is repeated multiple times and return error if so
     *  - the given command is either '--alg=merge' or '--alg=quick' and return error if not
     * If all the above cases pass, save the sorting algorithm
    */
    for(int i=0; i<argc; i++){
        if(strcmp(argv[i], "--name") == 0)
            cout<<"Vishnu Kumar\n";

        if(strcmp(argv[i], "-i") == 0){
            if(i_flag){
                cout<<"Multiple input files given. Please give only one input file\n";
                return 0;
            }
            if(i+1>=argc){
                cout<<"Please give an input file name and try again\n";
                return 0;
            }
            else{
                // converting character array to string using string
                ip_file = argv[++i];
                string ip_file_str = ip_file; 
                if(ip_file_str.find(".txt") == string::npos){
                    cout<<"Not a valid input file format. Please give a valid input file\n";
                    return 0;
                }
                i_flag = 1;
            }
        }
        
        if(strcmp(argv[i], "-o") == 0){
            if(o_flag){
                cout<<"Multiple output files given. Please give only one output file\n";
                return 0;
            }
            if(i+1>=argc){
                cout<<"Please give an output file name and try again\n";
                return 0;
            }
            else{
                op_file = argv[++i];
                o_flag = 1;
            }
        }
        
        string argv_str = argv[i];
        if(argv_str.find("-t") != string::npos){
            if(t_flag != 0){
                cout<<"Multiple threads i/p detected. Please choose only one\n";
                return 0;
            }

            argv_str = argv[++i];
            t_flag = atoi(&argv_str[0]);
        }

        if(argv_str.find("--alg") != string::npos){
            if(argv_str.find("=merge") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to merge sort\n";
                    alg_flag = 1;
                    continue;
                }
                alg_flag = 1;       
            }
            else if(argv_str.find("=quick") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to merge sort\n";
                    alg_flag = 1;
                    continue;
                }
                alg_flag = 2;
            }
            else{
                cout<<"Unknown sorting algorithm given. defaulting to merge sort\n";
                alg_flag = 1;
                return 0;
            }
        }
    }

    /**
     * After parsing through the command line inputs, check if we have recieved
     * an input file, an output file, number of threads to run the sorting
     * algorithm on, the sorting algorithm and run the requested sorting
     * algorithm. If an sorting algorithm is not given, default to merge sort.
     * If number of threads is not given, default to 4 threads. Raise
     * appropriate errors for each of the failed checks / if the sorting is 
     * failed.
    */
   
    if(ip_file == 0){
        cout<<"No input file given. Please give an output file and try again\n";
        return 0;
    }

    if(op_file == 0){
        cout<<"No output file given. Please give an output file and try again\n";
        return 0;
    }

    if(alg_flag == 0){
        alg_flag = 1; 
    }
    
    if(t_flag == 0){
        t_flag = 4;
    }
    
    // Try to open the given input and output files and return 0 if we are 
    // unable to open the files
    ifstream fin;
    fin.open(ip_file);
    if(!fin){
        cout<<"Unable to open \""<<ip_file<<"\" file. Please ensure the file exists and try again.\n";
        return 0;
    }

    ofstream fout;
    fout.open(op_file);
    if(!fout)
        return 0;
    
    vector<int> sort_arr;
        
    // Read each line of the input file, convert the number from char to int 
    // and save it to a vector array 
    string line;
    int x=0;

    while(getline(fin, line)){
        x = atoi(&line[0]);
        sort_arr.push_back(x);
    }

    int gh = sort_arr.size();
    
    //perform merge sort
    if(alg_flag == 1){
        if(merge_sort_openMP(sort_arr, 0, gh-1, t_flag) == 0){
            cout<<"Merge sort failed.";
            return 0;
        }
    }
    
    //perform quick sort
    if(alg_flag == 2){
        // if(quick_sort_openMP(sort_arr, 0, gh-1, t_flag) == 0){
        if(merge_sort_openMP2(sort_arr, 0, gh-1, t_flag, true) == 0){
            cout<<"Quick sort failed.";  
            return 0;
        }
    }

    // write back the sorted array into the given  output file.
    for(int i=0; i<gh; i++){
        fout<<sort_arr[i]<<"\n";       
    }

    fin.close();
    fout.close();
    return 1;
}