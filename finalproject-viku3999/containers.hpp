#include <stdio.h>
#include <iostream>

#include <pthread.h>  // Include POSIX threads for multi-threading
#include <atomic>      // Include atomic operations support for thread-safe data structures
#include <queue>       // Include the standard queue container
#include <stack>       // Include the standard stack container

using namespace std;  // Using the standard namespace to avoid prefixing std:: for common types like int, string, etc.

////////////////////////////////////////////////////////////////////////////////
// SGLQueue Class - Implements a single-lock queue
////////////////////////////////////////////////////////////////////////////////
class SGLQueue {
private:
    mutex sgl;           // Mutex for thread synchronization
    queue<int> sgl_queue; // The queue to hold integer elements

public:
    void enqueue(int val);  // Method to add an element to the queue
    int dequeue();          // Method to remove and return an element from the queue
};

////////////////////////////////////////////////////////////////////////////////
// SGLStack Class - Implements a stack with a single lock
////////////////////////////////////////////////////////////////////////////////
class SGLStack {
private:
    mutex sgl_lck;        // Mutex for thread synchronization
    stack<int> sgl_stack;  // The stack to hold integer elements

public:
    void push(int val);    // Method to add an element to the stack
    int pop();             // Method to remove and return an element from the stack
};

////////////////////////////////////////////////////////////////////////////////
// tStack Class - Implements a thread-safe stack using atomic pointers
////////////////////////////////////////////////////////////////////////////////
class tStack {
private:
    class node {
    public:
        atomic<int> val;   // Atomic value to store the element
        atomic<node*> down; // Atomic pointer to the next node (for stack linking)

        node(int v = 0, node* nextptr = nullptr) {
            val.store(v);     // Initialize value with atomic store
            down.store(nextptr); // Initialize next node pointer with atomic store
        }
    };
    atomic<node*> top;  // Atomic pointer to the top of the stack

public:
    void push(int val);  // Method to push an element onto the stack
    int pop();           // Method to pop an element from the stack
};

////////////////////////////////////////////////////////////////////////////////
// msqueue Class - Implements a lock-free multi-producer, multi-consumer queue
////////////////////////////////////////////////////////////////////////////////
class msqueue {
private:
    class node {
    public:
        int val;            // Value of the node
        atomic<node*> next; // Atomic pointer to the next node in the queue

        node(int v, node* nextptr = nullptr) {
            val = v;        // Initialize value
            next.store(nextptr);  // Initialize next pointer with atomic store
        }
    };

    atomic<node*> head, tail;  // Atomic pointers to the head and tail of the queue

public:
    msqueue();                 // Constructor to initialize the queue
    void enqueue(int val);     // Method to add an element to the queue
    int dequeue();             // Method to remove and return an element from the queue
};

////////////////////////////////////////////////////////////////////////////////
// elem_states Enum - Represents the different states of elements in data structures
////////////////////////////////////////////////////////////////////////////////
enum elem_states {
    PUSH,    // State for pushing an element
    POP,     // State for popping an element
    EMPTY,   // State when the element is empty
    OP_DONE, // State when operation is completed
    ENQ,     // State for enqueue operation
    DEQ      // State for dequeue operation
};

////////////////////////////////////////////////////////////////////////////////
// elem_arr Struct - Used to represent an element with its status and value
////////////////////////////////////////////////////////////////////////////////
struct elem_arr {
    atomic<int> status;  // Atomic variable to store the status of the element (e.g., PUSH, POP)
    int elem;            // The actual value of the element

    elem_arr() : status(EMPTY), elem(0) {}  // Default constructor initializes status to EMPTY
    elem_arr(int e) : status(EMPTY), elem(e) {}  // Constructor to initialize with a value

    // Copy constructor to handle deep copy and atomic status store
    elem_arr(const elem_arr& other) : status(EMPTY), elem(other.elem) {
        status.store(other.status.load());
    }

    // Copy assignment operator to ensure proper copying of element and status
    elem_arr& operator=(const elem_arr& other) {
        if (this != &other) {
            elem = other.elem;
            status.store(other.status.load());
        }
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////
// SGLStack_elem Class - Implements a stack with additional element array storage
////////////////////////////////////////////////////////////////////////////////
class SGLStack_elem {
private:
    atomic<bool> sgl_lck;      // Atomic lock flag for synchronization
    stack<int> sgl_stack;      // The stack to hold elements
    vector<elem_arr> arr;      // Vector to hold elements with their status

public:
    void push(int val);         // Method to push an element onto the stack
    int pop();                  // Method to pop an element from the stack
    void elem_arr_resize(int val);  // Method to resize the element array based on value

    SGLStack_elem() {
        sgl_lck.store(false);   // Initialize the lock flag to false (unlocked)
    }
};

////////////////////////////////////////////////////////////////////////////////
// tStack_elem Class - Implements a thread-safe stack with element array storage
////////////////////////////////////////////////////////////////////////////////
class tStack_elem {
private:
    class node {
    public:
        atomic<int> val;   // Atomic value of the node
        atomic<node*> down; // Atomic pointer to the next node in the stack

        node(int v = 0, node* nextptr = nullptr) {
            val.store(v);     // Initialize value with atomic store
            down.store(nextptr); // Initialize next node pointer with atomic store
        }
    };

    atomic<node*> top;    // Atomic pointer to the top of the stack
    vector<elem_arr> arr; // Vector to hold elements with their status

public:
    void push(int val);         // Method to push an element onto the stack
    int pop();                  // Method to pop an element from the stack
    void elem_arr_resize(int val);  // Method to resize the element array based on value

    tStack_elem() : top(nullptr) {} // Constructor initializes the top to nullptr
};

////////////////////////////////////////////////////////////////////////////////
// SGLStack_flat Class - Implements a stack with flat element array storage
////////////////////////////////////////////////////////////////////////////////
class SGLStack_flat {
private:
    atomic<bool> sgl_lck;     // Atomic lock flag for synchronization
    stack<int> sgl_stack;     // The stack to hold elements
    vector<elem_arr> arr;     // Vector to hold elements with their status

public:
    void push(int val);        // Method to push an element onto the stack
    int pop();                 // Method to pop an element from the stack
    void elem_arr_resize(int val);  // Method to resize the element array based on value

    SGLStack_flat() {
        sgl_lck.store(false);  // Initialize the lock flag to false (unlocked)
    }
};

////////////////////////////////////////////////////////////////////////////////
// SGLQueue_flat Class - Implements a queue with flat element array storage
////////////////////////////////////////////////////////////////////////////////
class SGLQueue_flat {
private:
    atomic<bool> sgl_lck;     // Atomic lock flag for synchronization
    queue<int> sgl_queue;     // The queue to hold elements
    vector<elem_arr> arr;     // Vector to hold elements with their status

public:
    void enqueue(int val);     // Method to enqueue an element
    int dequeue();             // Method to dequeue an element
    void elem_arr_resize(int val);  // Method to resize the element array based on value

    SGLQueue_flat() {
        sgl_lck.store(false);  // Initialize the lock flag to false (unlocked)
    }
};
