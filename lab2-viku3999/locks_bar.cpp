#include "locks_bar.hpp"
#include "thread"
#include <map>

#define SEQ_CST std::memory_order_seq_cst
#define RELAXED std::memory_order_relaxed
#define ACQ std::memory_order_acquire
#define REL std::memory_order_release

using namespace std;

thread_local Node* myNode;
thread_local int my_num = 0;

atomic<bool> flag(false);
atomic<int> next_num(0);
atomic<int> now_serving(0);

atomic<int> cnt(0);
atomic<int> sense(0);
int n;

atomic<bool> desires[2];
atomic<int> turn;
atomic<std::thread::id> t1_id, t2_id;

bool tas(atomic<bool> &x, std::memory_order MEM){
	bool expected = false;
	bool desired = true;
	return x.compare_exchange_strong(expected, desired, MEM);
}

int fai(atomic<int> &x, int amount, std::memory_order MEM){
	return x.fetch_add(amount, MEM);
}

template <typename T>
bool cas(atomic<T> &x, T expected, T desired, std::memory_order MEM){
	T expected_ref = expected;
	return x.compare_exchange_strong(expected_ref, desired, MEM);
}

Node::Node(){
	next.store(NULL, RELAXED);
	wait.store(false, SEQ_CST);
}

locks::locks(int lk_type, int NUM_THREADS){
	lock_type = lk_type;
	num_threads = NUM_THREADS;
	
	if((lock_type == PETERSON) && (num_threads != 2)){
		cout<<"Peterson lock only supports 2 threads. Hence, setting number of threads to 2\n";
        num_threads = 2;
	}

	counter_mutex = PTHREAD_MUTEX_INITIALIZER;
	tail = NULL;
	m = NULL;
	// oldTail = NULL;

	desires[0].store(false,SEQ_CST);
	desires[1].store(false,SEQ_CST);
	turn.store(0,SEQ_CST);
}

void print_tid(){
	thread::id a = this_thread::get_id();
	printf("t1: %ld", std::hash<std::thread::id>()(t1_id));
	printf(" t2: %ld\t", std::hash<std::thread::id>()(t2_id));
	printf("tid: %ld \n", std::hash<std::thread::id>()(a));
}

void Init_tid(){
	if((t1_id.load(ACQ) != std::thread::id()) &&
	   (t2_id.load(ACQ) != std::thread::id())){
			return;
	   }
	   	
	if((t1_id.load(ACQ) == std::thread::id()) &&
	   (t2_id.load(ACQ) == std::thread::id())){
		t1_id.store(this_thread::get_id(), REL);
		return;
	   }

	if((t1_id.load(ACQ) != std::thread::id()) &&
	   (t2_id.load(ACQ) == std::thread::id()) &&
	   (t1_id.load(ACQ) != this_thread::get_id())){
		t2_id.store(this_thread::get_id(), REL);
		return;
	   }
}

bool get_tid(){
	Init_tid();
	thread::id a = this_thread::get_id();
	// print_tid();

	bool ret = (a == t1_id)? 0 : 1;
	// printf("tid: %ld returning %d\n\n", std::hash<std::thread::id>()(a), ret);
	return ret;
}

thread_local atomic<bool> t(0);

void locks::lock(){
	switch (lock_type)
	{
	case TAS:
		while(tas(flag, SEQ_CST) == false){}
		break;
	
	case TTAS:
		while((flag.load(SEQ_CST) == true) ||
			(tas(flag, SEQ_CST) == false)){
			}
		break;

	case TICKET:
		my_num = fai(next_num, 1, SEQ_CST);
		while(now_serving.load(SEQ_CST)!=my_num){}
		break;

	case MCS:
		// myNode = new Node;

		oldTail = tail.load(SEQ_CST);
		// myNode->next.store(NULL, SEQ_CST);

		while(!cas(tail, oldTail, myNode, SEQ_CST)){
			oldTail = tail.load(SEQ_CST);
		}

		if(oldTail != NULL) {
			myNode->wait.store(true,RELAXED);
			oldTail->next.store(myNode,SEQ_CST);
			while (myNode->wait.load(SEQ_CST)) {}
		}

		break;
		
	case PTHREAD_LCK:
		pthread_mutex_lock(&counter_mutex);
		break;
	
	case PETERSON:
		t.store(get_tid(), SEQ_CST);

		// Say you want to acquire lock
		desires[t].store(true,SEQ_CST);

		// But, first give the other thread the chance to acquire lock
		turn.store(!t,SEQ_CST);
		
		// Wait until the other thread looses the desire to acquire lock or it
		// is your turn to get the lock.
		while(desires[!t].load(SEQ_CST)
		&& turn.load(SEQ_CST)==(!t)){}
		break;

	case TASREL:
		while(tas(flag, ACQ) == false){}
		break;
	
	case TTASREL:
		while((flag.load(ACQ) == true) ||
			(tas(flag, ACQ) == false)){
			}
		break;
	
	case MCSREL:
	
	case PETERSONREAL:
		t.store(get_tid(), REL);

		// Say you want to acquire lock
		desires[t].store(true,REL);

		// But, first give the other thread the chance to acquire lock
		turn.store(!t,REL);
		
		atomic_thread_fence(SEQ_CST);

		// Wait until the other thread looses the desire to acquire lock or it
		// is your turn to get the lock.
		while(desires[!t].load(ACQ)
		&& turn.load(ACQ)==(!t)){}
		break;

	default:
		pthread_mutex_lock(&counter_mutex);
		break;
	}
}

void locks::unlock(){
	switch (lock_type)
	{
	case TAS:
		flag.store(false, SEQ_CST);
		break;
	
	case TTAS:
		flag.store(false, SEQ_CST);
		break;

	case TICKET:
		fai(now_serving, 1,SEQ_CST);
		break;
	
	case MCS:
		m = myNode;

		if(tail.compare_exchange_strong(m, NULL, SEQ_CST)) {
			// no one is waiting, and we just
			// freed the lockx
			// printf("Lck relased\n");
		}

		else{// hand lock to next waiting thread
		while(myNode->next.load(SEQ_CST)==NULL){}
		// myNode->next->wait.store(false, SEQ_CST);
		m = myNode->next;
		m->wait.store(false, SEQ_CST);
			// printf("Lock relased\n");
		}
		
		break;

	case PTHREAD_LCK:
		pthread_mutex_unlock(&counter_mutex);
		break;
		
	case PETERSON:
		t.store(get_tid(), SEQ_CST);
		desires[t].store(false,SEQ_CST);
		break;

	case TASREL:
		flag.store(false, REL);
		break;
	
	case TTASREL:
		flag.store(false, REL);
		break;
	
	case MCSREL:
	
	case PETERSONREAL:
		t.store(get_tid(), REL);
		desires[t].store(false,REL);
		break;
	
	default:
		pthread_mutex_unlock(&counter_mutex);
		break;
	}
}

void Sense_bar_rel(){
	thread_local bool my_sense=0;

	if(my_sense==0){
		my_sense=1;
	} //flip sense
	else{
		my_sense=0;
	}
	
	int cnt_cpy = fai(cnt, 1, ACQ);
	
	if(cnt_cpy == (n-1)){ //last to arrive
	
		cnt.store(0, RELAXED);
		sense.store(my_sense,RELAXED);
	}
	else{ //not last
		while(sense.load(ACQ)!=my_sense){}
	}
}

void Sense_bar(){
	thread_local bool my_sense=0;

	if(my_sense==0){
		my_sense=1;
	} //flip sense
	else{
		my_sense=0;
	}
	
	int cnt_cpy = fai(cnt, 1, SEQ_CST);
	
	if(cnt_cpy == (n-1)){ //last to arrive
	
		cnt.store(0, RELAXED);
		sense.store(my_sense,SEQ_CST);
	}
	else{ //not last
		while(sense.load(SEQ_CST)!=my_sense){}
	}
}

barriers::barriers(int barrier_type, int NUM_THREADS){
	bar_type = barrier_type;
	num_threads = NUM_THREADS;
	n = NUM_THREADS;
	pthread_barrier_init(&pthread_bar, NULL, num_threads);
}

void barriers::arrive_wait(){
	switch (bar_type)
	{
	case SENSE:
		// printf("n: %d\t ", n);
		Sense_bar();
		break;
	
	case PTHREAD_BAR:
		pthread_barrier_wait(&pthread_bar);	
		break;
	
	case SENSEREL:
		Sense_bar_rel();
		break;
	default:
		pthread_barrier_wait(&pthread_bar);
		break;
	}
}