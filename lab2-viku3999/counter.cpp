#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>

#include <getopt.h>
#include <vector>
#include <thread>

#include <ctime>
#include <climits>

#include "locks_bar.hpp"

using namespace std;

int global_ctr = 0;

vector<thread*> threads;
struct timespec startTime, endTime;
size_t NUM_THREADS;

locks* lck;
barriers* bar;

void bar_init(int bar_type){
    bar = new barriers(bar_type, NUM_THREADS);
}

void lock_init(int lock_type){
    lck = new locks(lock_type, NUM_THREADS);
}

void global_cleanup(){
	delete lck;

}

void lock_counter(int num_itr){
    for(int i=0; i<num_itr; i++){
        lck->lock();
        global_ctr++;
        lck->unlock();
    }
}

void bar_counter(int tid, int num_itr){
    for(int i = 0; i<num_itr*NUM_THREADS; i++){
		if(i%NUM_THREADS==tid){
			global_ctr++;
		}
        bar->arrive_wait();
	}
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
 * @brief   Main function to carry out the program requiremnts.
 * @return  1 -> if program is successfull
 *          0 -> if program is uncuccessfull
 */
int main(int argc, char *argv[]){

    char* ip_file=0;
    char* op_file=0;  
    int num_flag=0, o_flag=0, t_flag=0, bar_flag=0, lock_flag=0;

    string bar_val[] = {"sense", "pthread", "senserel"};
    string lock_val[] = {"tas","ttas","ticket","mcs","pthread","peterson","tasrel","ttasrel","mcsrel","petersonrel"};

    /**
     * Iterate through the command line arguments
     * 
     * If we recieve '--name', print out your name
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
        
        string argv_str = argv[i];

        if(argv_str.find("-t") != string::npos){
            if(t_flag != 0){
                cout<<"Multiple threads i/p detected. Please choose only one\n";
                return 0;
            }
            argv_str = argv[++i];
            t_flag = atoi(&argv_str[0]);
			
			// cout<<"Got: "<<t_flag<<" in -t option\n";
        }

        if(argv_str.find("-n") != string::npos){
            if(num_flag != 0){
                cout<<"Multiple number i/p detected. Please choose only one\n";
                return 0;
            }
            argv_str = argv[++i];
            num_flag = atoi(&argv_str[0]);
			
			// cout<<"Got: "<<num_flag<<" in -n option\n";
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
                if(lock_flag == MCSREL){
                    cout<<"Lock not supported. Using pthread lock instead";
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
                // cout<<"Got op file: "<<op_file<<"\n";
            }
        }

    }

    // /**
    //  * After parsing through the command line inputs, check if we have recieved
    //  * an input file, an output file and an sorting algorithm and run the 
    //  * requested sorting algorithm. Raise appropriate errors for each of the 
    //  * failed checks / if the sorting is failed
    // */

    if(op_file == 0){
        cout<<"No output file given. Please give an output file and try again\n";
        return 0;
    }

    if((bar_flag != 0) && (lock_flag != 0)){
        cout<<"Please give only one barrier/locking algorithm as input\n";
        return 0;
    }

    if(num_flag == 0){
        cout<<"Please give number of iterations\n";
        return 0;
    }
    
    if(t_flag == 0){
        t_flag = 4;
    }

    if((lock_flag == PETERSON) && (t_flag != 2)){
        cout<<"Peterson lock only supports 2 threads. Hence, setting number of threads to 2\n";
        t_flag = 2;
    }

    NUM_THREADS = t_flag;

    ofstream fout;
    fout.open(op_file);
    if(!fout)
        return 0;
    
    
    threads.resize(t_flag);
    clock_gettime(CLOCK_MONOTONIC,&startTime);

    if(lock_flag != 0){
        lock_init(lock_flag);
        for(int i=1; i<t_flag; i++){
            threads[i] = new thread(lock_counter, num_flag);
        }

        lock_counter(num_flag);
    }

    if(bar_flag != 0){
        bar_init(bar_flag);
        for(int i=1; i<t_flag; i++){
            threads[i] = new thread(bar_counter, i, num_flag);
        }
        bar_counter(0, num_flag);
    }
    
	// join threads
	for(size_t i=1; i<t_flag; i++){
		threads[i]->join();
		delete threads[i];
	}
	global_cleanup();

	clock_gettime(CLOCK_MONOTONIC,&endTime);
	print_time();

    fout<<global_ctr<<"\n";       

    fout.close();
    return 1;
}