#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <thread>
#include <vector>
#include <bits/stdc++.h>

#include "containers.hpp"

// Define various constants for different algorithms and data structures.
#define SGL_STACK 1
#define SGL_QUEUE 2
#define TREIBER_STACK 3
#define MS_QUEUE 4
#define TREIBER_STACK_ELEM 5
#define SGL_STACK_ELEM 6
#define SGL_STACK_FLAT 7
#define SGL_QUEUE_FLAT 8

#define STACK 6
#define QUEUE 7

using namespace std;

// Global variables for output array, threads, and various data structures.
int *out_arr = NULL;
vector<thread*> threads;

SGLQueue queue_sgl;
SGLStack stack_sgl;
tStack Stack_t;
msqueue queue_ms;

SGLStack_elem stack_sgl_elem;
tStack_elem Stack_t_elem;

SGLStack_flat stack_sgl_flat;
SGLQueue_flat queue_sgl_flat;

void* SGL_Stack_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        stack_sgl.push(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = stack_sgl.pop();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* SGL_Queue_Test(int start, int stop, int tid){    
    for(int i=start; i<=stop; i++){
        queue_sgl.enqueue(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = queue_sgl.dequeue();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* SGL_Queue_Flat_Test(int start, int stop, int tid){    
    for(int i=start; i<=stop; i++){
        queue_sgl_flat.enqueue(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = queue_sgl_flat.dequeue();
        out_arr[i-1] = ret;
    }
    return 0;
}
void* SGL_Stack_Elem_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        stack_sgl_flat.push(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = stack_sgl_flat.pop();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* SGL_Stack_Flat_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        stack_sgl_elem.push(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = stack_sgl_elem.pop();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* TREIBER_Stack_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        Stack_t.push(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = Stack_t.pop();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* TREIBER_Stack_Elem_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        Stack_t_elem.push(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = Stack_t_elem.pop();
        out_arr[i-1] = ret;
    }
    return 0;
}

void* MS_Queue_Test(int start, int stop, int tid){
    for(int i=start; i<=stop; i++){
        queue_ms.enqueue(i);
    }

    int ret=-1;
    for(int i=start; i<=stop; i++){
        ret = queue_ms.dequeue();
        out_arr[i-1] = ret;
    }
    return 0;
}

bool comp(int a, int b) {
      return a > b;
}

/**
 * @brief   Main function to carry out the program requiremnts.
 * @return  1 -> if program is successfull
 *          0 -> if program is uncuccessfull
 */
int main(int argc, char *argv[]){

    char* op_file=0;  
    int o_flag=0;
    int t_flag=0, alg_flag=0, n_flag=0;
    int sort_dir = -1;
    
    // Iterate through the command line arguments
    for(int i=0; i<argc; i++){
        string argv_str = argv[i];
        
        // Help cmd
        if(argv_str.find("-h") != string::npos){
                cout<<"Concurrent Containers. Supported commands.\n";
                cout<<"\t -h : Bringup the help command screen (optional command)\n";
                cout<<"\t -t <num> : give the number of threads to launch.\n";
                cout<<"\t -n <num> : give number of elements to push to the stack/queue in across all threads.\n";
                cout<<"\t -o <output_file_name> : output file to which we should write the pop/dequeue results to\n";
                cout<<"\t --alg=<sgl_queue, sgl_queue_flat, sgl_stack, sgl_stack_elem, sgl_stack_flat, treiber, treiber_elem, ms> : stack/queue algorithm to run.\n";
        }

        // num threads
        if(argv_str.find("-t") != string::npos){
            if(t_flag != 0){
                cout<<"Multiple threads i/p detected. Defaulting to 4\n";
                t_flag = 4;
            }

            argv_str = argv[++i];
            t_flag = atoi(&argv_str[0]);
        }

        // Num of elements to use
        if(argv_str.find("-n") != string::npos){
            if(n_flag != 0){
                cout<<"Multiple num of counts i/p detected. Please choose only one\n";
                return 0;
            }

            argv_str = argv[++i];
            n_flag = atoi(&argv_str[0]);
        }
        
        // Output file
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

        // Data structure to use
        if(argv_str.find("--alg") != string::npos){
            if(argv_str.find("=sgl_queue") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = SGL_QUEUE;       
            }
            else if(argv_str.find("=sgl_queue_flat") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = SGL_QUEUE_FLAT;
            }
            else if(argv_str.find("=sgl_stack") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = SGL_STACK;
            }
            else if(argv_str.find("=sgl_stack_elem") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = SGL_STACK_ELEM;
            }
            else if(argv_str.find("=sgl_stack_flat") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = SGL_STACK_FLAT;
            }
            else if(argv_str.find("=treiber") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = TREIBER_STACK;       
            }
            else if(argv_str.find("=treiber_elem") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = TREIBER_STACK_ELEM;       
            }
            else if(argv_str.find("=ms") != string::npos){
                if(alg_flag){
                    cout<<"Multiple sorting algorithms detected. defaulting to sgl queue\n";
                    alg_flag = SGL_QUEUE;
                    continue;
                }
                alg_flag = MS_QUEUE;
            }
            else{
                cout<<"Unknown sorting algorithm given. defaulting to sgl queue\n";
                alg_flag = SGL_QUEUE;
            }
        }
    }


    // Set default param if needed
    if(alg_flag == 0){
        alg_flag = SGL_QUEUE; 
    }
    
    if(t_flag == 0){
        t_flag = 4;
    }

    if(op_file == 0){
        cout<<"No output file given. Please give an output file and try again\n";
        return 0;
    }
    
    ofstream fout;
    fout.open(op_file);
    if(!fout)
        return 0;

    out_arr = (int*) malloc(n_flag * sizeof(int));

    
    int count_size = n_flag / t_flag;
    int i=0;

	threads.resize(t_flag);

    switch (alg_flag)
    {
    case SGL_QUEUE:
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }

                threads[i] = new thread(SGL_Queue_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = QUEUE;

        break;
    case SGL_QUEUE_FLAT:
            queue_sgl_flat.elem_arr_resize(t_flag);
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }
                threads[i] = new thread(SGL_Queue_Flat_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case SGL_STACK:
        	for(i=0; i<t_flag; i++){
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }

                threads[i] = new thread(SGL_Stack_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case SGL_STACK_ELEM:
            stack_sgl_elem.elem_arr_resize(t_flag);
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }

                threads[i] = new thread(SGL_Stack_Elem_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case SGL_STACK_FLAT:
            stack_sgl_flat.elem_arr_resize(t_flag);
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }

                threads[i] = new thread(SGL_Stack_Flat_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case TREIBER_STACK:
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }


                threads[i] = new thread(TREIBER_Stack_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case TREIBER_STACK_ELEM:
            Stack_t_elem.elem_arr_resize(t_flag);
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }

                threads[i] = new thread(TREIBER_Stack_Elem_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = STACK;
        break;
    case MS_QUEUE:
        	for(i=0; i<t_flag; i++){        
                int l_start = (i*count_size)+1;
                int l_end = (i+1)*count_size;

                if((n_flag - l_end)<count_size){
                    l_end = n_flag;
                }


                threads[i] = new thread(MS_Queue_Test, l_start, l_end, i);
            }
            
        	// join threads
            for(size_t i=1; i<t_flag; i++){
                threads[i]->join();
                delete threads[i];
            }
            sort_dir = QUEUE;
        break;
    default:
        break;
    }

    switch (sort_dir)
    {
    case STACK:
        sort(out_arr, out_arr + n_flag, comp);
        break;
    case QUEUE:
        sort(out_arr, out_arr + n_flag);
        break;

    default:
        break;
    }

    // write back the sorted array into the given  output file.
    for(int i=0; i<n_flag; i++){
        fout<<out_arr[i]<<"\n";       
    }

    fout.close();

    return 1;
}