#include <iostream>
using namespace std;

#define SIZE 8

template <class Qtype> class q_type {
  Qtype queue[SIZE]; 
  int head, tail;    
public:
  q_type() { 
     head = tail = 0; 
  }
  void q(Qtype num); 
  Qtype deq();  
};

template <class Qtype> void q_type<Qtype>::q(Qtype num)
{
  if(tail+1==head || (tail+1==SIZE && !head)) {
    cout << "Queue is full. I'm flushing all but one item.\n";
    // Queue full means we are getting way too much data
    // and we should just dump it all but the last bit
    // just point the head at the last entry and continue
    head = tail;
  }
  tail++;
  if(tail==SIZE) 
     tail = 0; // cycle around
  queue[tail] = num;
  cout << "Q size " << ((tail>head)?(tail-head):(SIZE-head)+tail) << endl;
}

template <class Qtype> Qtype q_type<Qtype>::deq()
{
  if(head == tail) { return 0;  }
  head++;
  if(head==SIZE) 
     head = 0; 
  return queue[head];
}




