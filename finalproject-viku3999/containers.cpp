#include "containers.hpp"
#include <random>
#include <chrono>
#include <thread>

using namespace std;

#define SEQ_CST std::memory_order_seq_cst
#define RELAXED std::memory_order_relaxed
#define ACQ std::memory_order_acquire
#define REL std::memory_order_release
#define ACQ_REL std::memory_order_acq_rel

#define DUMMY 0

template <typename T>
bool CAS(atomic<T> &x, T expected, T desired, std::memory_order MEM){
	T expected_ref = expected;
	return x.compare_exchange_strong(expected_ref, desired, MEM);
}

void SGLQueue::enqueue(int val){
	sgl.lock();
	sgl_queue.push(val);
	sgl.unlock();
}

int SGLQueue::dequeue(){
	sgl.lock();
	int ret_val = sgl_queue.front();
	sgl_queue.pop();
	sgl.unlock();
	return ret_val;
}

void SGLStack::push(int val){
	sgl_lck.lock();
	sgl_stack.push(val);
	sgl_lck.unlock();
}

int SGLStack::pop(){
	sgl_lck.lock();
	
	int ret_val = sgl_stack.top();
	sgl_stack.pop();

	sgl_lck.unlock();

	return ret_val;
}

void tStack::push(int val){
	node* n = new node(val);
	node* t = new node(0);
	do{
		t = top.load(ACQ);
		n->down = t;
	}while(!CAS(top, t, n, REL));
}

int tStack::pop(){
	int v;
	node* t = new node(0);
	node* n = new node(0);
	do{
		t = top.load(ACQ);
		
		if(t==NULL){
			return NULL;
		}

		n = t->down;
		v = t->val;
	}while(!CAS(top, t, n, REL));

	return v;
}

msqueue::msqueue(){
	node* dummy = new node(DUMMY, nullptr);
	head.store(dummy, RELAXED);
	tail.store(dummy, RELAXED);
}

void msqueue::enqueue(int val){
	node *t = new node(DUMMY, nullptr);
	node *e = new node(DUMMY, nullptr);
	node *n = new node(val, nullptr);

	while(true){
		t = tail.load(); 
		e = t->next.load();
		if(t == tail.load()){
			if(e==NULL && CAS(t->next, e, n, RELAXED)){
				break;
			}
			else if(e!=NULL){
				CAS(tail, t, e, RELAXED);
			}
		}
	}
	CAS(tail, t, n, RELAXED);
}

int msqueue::dequeue(){
	node *t, *h, *n;

	while(true){

		h=head.load(); t=tail.load(); n=h->next.load();
		
		if(h==head.load()){
			if(h==t){
				if(n==NULL){
					return NULL;
				}
				else{
					CAS(tail, t, n, RELAXED);
				}
			}
			else{
				int ret=n->val;
				if(CAS(head, h, n, RELAXED)){
					return ret;
				}
			}
		}
	}
}

void tStack_elem::elem_arr_resize(int val){
	elem_arr x(0);
	for(int i=0; i<val; i++){
		arr.push_back(x);
	}
}

void tStack_elem::push(int val){
	node* n = new node(val);
	node* top_node = top.load(ACQ);
	n->down = top_node;

	while(true){

		// Elem process:
		// try top CAS 
		if(CAS(top, top_node, n, REL)){
			return;
		}

		// If failed: 
		// pick a random slot in elem array
		random_device rd;
        static thread_local mt19937 gen(rd());
        uniform_int_distribution<> distrib(0, arr.size() - 1);
        int index = distrib(gen);

		// If it’s empty, place details of your operation there with CAS and wait a little
        if (CAS(arr[index].status, (int)EMPTY, (int)PUSH, ACQ_REL)) {

            arr[index].elem = val;  // Store the val
            this_thread::sleep_for(chrono::nanoseconds(10));  // Allow time for a matching pop
            
			
			// If non-empty and of an opposite type, CAS in your details to exchange
			if (CAS(arr[index].status, (int)POP, (int)EMPTY, ACQ_REL)) {
                // val was consumed, cleanup and exit
                delete n;
                return;
            } 
			else {
				
				// If it’s non-empty and not usable (completed or same type operation), pick a different slot
                arr[index].status.store(EMPTY, REL);  // Reset the slot
            }
        }

        // Update temp->next in case the stack top changed during elimination
		top_node = top.load(ACQ);
        n->down = top_node;
	
		// If no one combines, take your operation out and retry stack

	}
}

int tStack_elem::pop(){
	int ret_val;
	node* top_node = new node(0);
	node* n = new node(0);
	// pick a random slot in elem array
	random_device rd;
	static thread_local mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, arr.size() - 1);

	while(true){

		top_node = top.load(ACQ);

		if(top_node==NULL){
			// Check elem array
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)PUSH, (int)POP, ACQ_REL)){
				ret_val = arr[index].elem;
				return ret_val;
			}

			return NULL;
		}

		n = top_node->down;
		ret_val = top_node->val;
		if(CAS(top, top_node, n, REL)){
			return ret_val;
		}
		else{
			// Check elem array
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)PUSH, (int)POP, ACQ_REL)){
				ret_val = arr[index].elem;
				return ret_val;
			}
		}
	}
}

void SGLStack_elem::elem_arr_resize(int val){
	elem_arr x(0);
	for(int i=0; i<val; i++){
		arr.push_back(x);
	}
}

void SGLStack_elem::push(int val){
	random_device rd;
	static thread_local mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, arr.size() - 1);
	
	while(true){

		// if we got lock, push the value to the stack and release the lock
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			sgl_stack.push(val);
			sgl_lck.store(false);
			return;
		}

		// Else, do elimination process
		int index = distrib(gen);
		
		// If it’s empty, place details of your operation there with CAS and wait a little
        if (CAS(arr[index].status, (int)EMPTY, (int)PUSH, ACQ_REL)) {

            arr[index].elem = val;  // Store the val
            this_thread::sleep_for(chrono::nanoseconds(10));  // Allow time for a matching pop
			
			if (CAS(arr[index].status, (int)POP, (int)EMPTY, ACQ_REL)) {
                // val was consumed, cleanup and exit
                return;
            } 
			else {
				// If it’s non-empty and not usable (completed or same type operation), pick a different slot
                arr[index].status.store(EMPTY, REL);  // Reset the slot
            }
        }
		// If non-empty and of an opposite type, CAS in your details to exchange
		else if(CAS(arr[index].status, (int)POP, (int)PUSH, ACQ_REL)){
			arr[index].elem = val;
		}
		else{
			continue;
		}

	}
}

int SGLStack_elem::pop(){
	random_device rd;
	static thread_local mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, arr.size() - 1);
	int ret_val = -1;
	
	while(true){
		// if we got lock, push the value to the stack and release the lock
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			ret_val = sgl_stack.top();
			sgl_stack.pop();
			sgl_lck.store(false);

			if(ret_val != 0)
				break;
		}

		// Else, do elimination process
		int index = distrib(gen);
		
		// Check if the corresponding index has a push operation queued
        if (CAS(arr[index].status, (int)PUSH, (int)POP, ACQ_REL)) {

			ret_val = arr[index].elem;          
			break;
		}
		// If Empty, CAS in your details to exchange
		else if (CAS(arr[index].status, (int) EMPTY, (int) POP, ACQ_REL)){
            this_thread::sleep_for(chrono::nanoseconds(10));  // Allow time for a matching push

			if(CAS(arr[index].status, (int)PUSH, (int)EMPTY, ACQ_REL)){
				ret_val = arr[index].elem;
				break;
			}
		}
		// If it’s non-empty and not usable (completed or same type operation), pick a different slot
	}

	return ret_val;
}

void SGLStack_flat::elem_arr_resize(int val){
	elem_arr x(0);
	for(int i=0; i<val; i++){
		arr.push_back(x);
	}
}

void SGLStack_flat::push(int val){

	while(true){
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			sgl_stack.push(val);
			
			// go through the elem array and perform the PUSH/POP actions if present.
			for(int i=0; i<arr.size(); i++){
				// EMPTY operation

				if(arr[i].status.load(ACQ_REL) == EMPTY){
					continue;
				}

				// PUSH operation
				if(arr[i].status.load(ACQ_REL) == PUSH){
					// Searching for a matching POP operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == POP){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[j].elem = arr[i].elem;
							match=true;
							break;
						}
					}

					// If no match found, push the element to the stack
					if(!match){
						sgl_stack.push(arr[i].elem);
						arr[i].status = OP_DONE;
						arr[i].elem = 0;
					}
				}

				// POP operation
				if(arr[i].status.load(ACQ_REL) == POP){
					//Searching for a matching PUSH operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == PUSH){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[i].elem = arr[j].elem;
							match=true;
							break;
						}
					}

					// If no match found, POP an element from the stack
					if(!match){
						arr[i].elem = sgl_stack.top();
						sgl_stack.pop();
						arr[i].status = OP_DONE;
					}
				}

			}

			// release lock
			sgl_lck.store(false);
		}

		// If unable to get lock, insert operation to the flat combining array
		else{
			random_device rd;
			static thread_local mt19937 gen(rd());
			uniform_int_distribution<> distrib(0, arr.size() - 1);
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)EMPTY, (int)PUSH, ACQ_REL)){
				arr[index].elem = val;
				this_thread::sleep_for(chrono::nanoseconds(5));  // Allow time for a flat combining op to complete

				// If op is completed by the thread holding lock, return. else, retry push operation from begigning
				if(arr[index].status == OP_DONE){
					return;
				}
			}
		}
	}

}

int SGLStack_flat::pop(){
	int ret_val = -1;

	while(true){
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			ret_val = sgl_stack.top();
			sgl_stack.pop();
			
			// go through the elem array and perform the PUSH/POP actions if present.
			for(int i=0; i<arr.size(); i++){
				// EMPTY operation
				if(arr[i].status.load(ACQ_REL) == EMPTY){
					continue;
				}

				// PUSH operation
				if(arr[i].status.load(ACQ_REL) == PUSH){
					// Searching for a matching POP operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == POP){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[j].elem = arr[i].elem;
							match=true;
							break;
						}
					}

					// If no match found, push the element to the stack
					if(!match){
						sgl_stack.push(arr[i].elem);
						arr[i].status = OP_DONE;
						arr[i].elem = 0;
					}
				}

				// POP operation
				if(arr[i].status.load(ACQ_REL) == POP){
					//Searching for a matching PUSH operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == PUSH){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[i].elem = arr[j].elem;
							match=true;
							break;
						}
					}

					// If no match found, POP an element from the stack
					if(!match){
						arr[i].elem = sgl_stack.top();
						sgl_stack.pop();
						arr[i].status = OP_DONE;
					}
				}

			}

			// release lock
			sgl_lck.store(false);
		}

		// If unable to get lock, insert operation to the flat combining array
		else{
			random_device rd;
			static thread_local mt19937 gen(rd());
			uniform_int_distribution<> distrib(0, arr.size() - 1);
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)EMPTY, (int)POP, ACQ_REL)){
				this_thread::sleep_for(chrono::nanoseconds(5));  // Allow time for a flat combining op to complete

				// If op is completed by the thread holding lock, return. else, retry pop operation from begigning
				if(arr[index].status == OP_DONE){
					ret_val = arr[index].elem;
					arr[index].elem = 0;
					arr[index].status = EMPTY;
					return ret_val;
					break;
				}
			}
		}
	}
	return ret_val;
}

void SGLQueue_flat::elem_arr_resize(int val){
	elem_arr x(0);
	for(int i=0; i<val; i++){
		arr.push_back(x);
	}
}

void SGLQueue_flat::enqueue(int val){
	
	while(true){
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			sgl_queue.push(val);
			
			// go through the elem array and perform the ENQ/DEQ actions if present.
			for(int i=0; i<arr.size(); i++){
				// EMPTY operation
				if(arr[i].status.load(ACQ_REL) == EMPTY){
					continue;
				}

				// Enqueue operation
				if(arr[i].status.load(ACQ_REL) == ENQ){
					// Searching for a matching Dequeue operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == DEQ){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[j].elem = arr[i].elem;
							match=true;
							break;
						}
					}

					// If no match found, push the element to the stack
					if(!match){
						sgl_queue.push(val);
						arr[i].status = OP_DONE;
						arr[i].elem = 0;
					}
				}

				// Dequeue operation
				if(arr[i].status.load(ACQ_REL) == DEQ){
					//Searching for a matching Enqueue operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == ENQ){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[i].elem = arr[j].elem;
							match=true;
							break;
						}
					}

					// If no match found, dequeue an element from the queue
					if(!match){
						arr[i].elem = sgl_queue.front();
						sgl_queue.pop();
						arr[i].status = OP_DONE;
					}
				}

			}

			// release lock
			sgl_lck.store(false);
		}

		// If unable to get lock, insert operation to the flat combining array
		else{
			random_device rd;
			static thread_local mt19937 gen(rd());
			uniform_int_distribution<> distrib(0, arr.size() - 1);
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)EMPTY, (int)ENQ, ACQ_REL)){
				arr[index].elem = val;
				this_thread::sleep_for(chrono::nanoseconds(5));  // Allow time for a flat combining op to complete

				// If op is completed by the thread holding lock, return. else, retry push operation from begigning
				if(arr[index].status == OP_DONE){
					return;
				}
			}
		}
	}
}

int SGLQueue_flat::dequeue(){	
	int ret_val = -1;

	while(true){
		if(CAS(sgl_lck, false, true, ACQ_REL)){
			ret_val = sgl_queue.front();
			sgl_queue.pop();
			
			// go through the elem array and perform the ENQ/DEQ actions if present.
			for(int i=0; i<arr.size(); i++){
				// EMPTY operation
				if(arr[i].status.load(ACQ_REL) == EMPTY){
					continue;
				}

				// Enqueue operation
				if(arr[i].status.load(ACQ_REL) == ENQ){
					// Searching for a matching Dequeue operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == DEQ){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[j].elem = arr[i].elem;
							match=true;
							break;
						}
					}

					// If no match found, push the element to the stack
					if(!match){
						int val = arr[i].elem;
						sgl_queue.push(val);
						arr[i].status = OP_DONE;
						arr[i].elem = 0;
					}
				}

				// Dequeue operation
				if(arr[i].status.load(ACQ_REL) == DEQ){
					//Searching for a matching Enqueue operation
					bool match=false;
					for(int j=0; j<arr.size(); j++){
						if(arr[j].status.load(ACQ_REL) == ENQ){
							arr[i].status = OP_DONE;
							arr[j].status = OP_DONE;
							arr[i].elem = arr[j].elem;
							match=true;
							break;
						}
					}

					// If no match found, POP an element from the stack
					if(!match){
						
						arr[i].elem = sgl_queue.front();
						sgl_queue.pop();
						arr[i].status = OP_DONE;
					}
				}

			}

			// release lock
			sgl_lck.store(false);
		}

		// If unable to get lock, insert operation to the flat combining array
		else{
			random_device rd;
			static thread_local mt19937 gen(rd());
			uniform_int_distribution<> distrib(0, arr.size() - 1);
			int index = distrib(gen);

			if(CAS(arr[index].status, (int)EMPTY, (int)DEQ, ACQ_REL)){
				this_thread::sleep_for(chrono::nanoseconds(5));  // Allow time for a flat combining op to complete

				// If op is completed by the thread holding lock, return. else, retry pop operation from begigning
				if(arr[index].status == OP_DONE){
					ret_val = arr[index].elem;
					arr[index].elem = 0;
					arr[index].status = EMPTY;
					return ret_val;
					break;
				}
			}
		}
	}
	return ret_val;
}