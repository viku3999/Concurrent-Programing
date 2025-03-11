// Macros for locks 
#define TAS				1
#define TTAS			2
#define TICKET			3
#define MCS				4
#define PTHREAD_LCK		5
#define PETERSON		6
#define TASREL			7
#define TTASREL			8
#define MCSREL			9
#define PETERSONREAL	10

// Macros for barriers
#define SENSE			1
#define PTHREAD_BAR		2
#define SENSEREL		3

#include <pthread.h>
#include <atomic>
#include <iostream>

using namespace std;

class barriers{
	public:
		barriers(int barrier_type, int NUM_THREADS);
		void arrive_wait();

	private:
		int bar_type;
		int num_threads;
		pthread_barrier_t pthread_bar;
};

class Node{
	public:
		atomic<Node*> next;
		atomic<bool> wait;

		Node();
};


class locks{
	public:
		locks(int lk_type, int NUM_THREADS);
		void lock();
		void unlock();

	private:
		int lock_type;
		int num_threads;
		pthread_mutex_t counter_mutex;
		atomic<Node*> tail;
		Node* oldTail;
		Node* m;
};