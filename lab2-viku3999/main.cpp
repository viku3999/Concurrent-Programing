#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Sort_p.hpp"
#include "locks_bar.hpp"

using namespace std;

/**
 * @brief   Main function to carry out the program requiremnts.
 * @return  1 -> if program is successfull
 *          0 -> if program is uncuccessfull
 */
int main(int argc, char *argv[]){
    
    char* ip_file=0;
    char* op_file=0;  
    int i_flag=0, o_flag=0, t_flag=0, alg_flag=0, bar_flag=0, lock_flag=0;

    string bar_val[] = {"sense", "pthread"};
    string lock_val[] = {"tas","ttas","ticket","mcs","pthread","petersonseq","petersonrel"};

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
        // cout<<argv[i]<<"\n";
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

            // char *num = &argv_str[2];
            // t_flag = atoi(num);
            // cout<<"\n"<<t_flag;
        }

        if(argv_str.find("--alg") != string::npos){
            if(argv_str.find("=forkjoin") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. Please choose only one\n";
                    return 0;
                }
                alg_flag = 1;       
            }
            else if(argv_str.find("=lkbucket") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. Please choose only one\n";
                    return 0;
                }
                alg_flag = 2;
            }
            else{
                cout<<"Unknown sorting algorithm given. Please check command\n";
                return 0;
            }
        }
    
        if(argv_str.find("--bar=") != string::npos){
            int pos = argv_str.find("=");
            string sub_string = argv_str.substr(pos+1);

            for(int i=0; i<sizeof(bar_val); i++){
                if(sub_string == bar_val[i]){
                    if(bar_flag){
                        cout<<"Multiple barrier methods detected. Please choose only one\n";
                        return 0;
                    }
                bar_flag = i+1;
                // cout<<"Got bar: "<<bar_flag<<"\n";
                break;
                }
            }
            if(bar_flag == 0){
                cout<<"Unknown barrier method\n";
            }
        }

        if(argv_str.find("--lock=") != string::npos){
            int pos = argv_str.find("=");
            string sub_string = argv_str.substr(pos+1);

            for(int i=0; i<sizeof(lock_val); i++){
                if(sub_string == lock_val[i]){
                    if(lock_flag){
                    cout<<"Multiple lock methods detected. Please choose only one\n";
                    return 0;
                    }
                lock_flag = i+1;
                if(lock_flag == 7){
                    lock_flag = PETERSONREAL;
                }
                // cout<<"Got lock: "<<lock_flag<<"\n";
                break;
                }
            }
            if(lock_flag == 0){
                cout<<"Unknown lock method\n";
                return 0;
            }
        }
    }
    
    /**
     * After parsing through the command line inputs, check if we have recieved
     * an input file, an output file and an sorting algorithm and run the 
     * requested sorting algorithm. Raise appropriate errors for each of the 
     * failed checks / if the sorting is failed
    */
   
    if(ip_file == 0){
        cout<<"No input file given. Please give an output file and try again\n";
        return 0;
    }

    if(op_file == 0){
        cout<<"No output file given. Please give an output file and try again\n";
        return 0;
    }

    if((bar_flag != 0) && (lock_flag != 0)){
        cout<<"Please give only one barrier/locking algorithm as input\n";
        return 0;
    }

    if((bar_flag == 0) && (lock_flag == 0)){
        bar_flag = PTHREAD_LCK;
        return 0;
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
    
    if(lock_flag != 0){
        if(lkbucket_lock(sort_arr, 0, gh-1, t_flag, lock_flag) == 0){
            cout<<"Bucket sort failed.\n";  
            return 0;
        }
    }

    if(bar_flag != 0){
        if(lkbucket_bar(sort_arr, 0, gh-1, t_flag, bar_flag) == 0){
            cout<<"Bucket sort failed.";  
            return 0;
        }
    }

    for(int i=0; i<gh; i++){
        fout<<sort_arr[i]<<"\n";       
    }

    fin.close();
    fout.close();
    return 1;
}