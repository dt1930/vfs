#include <iostream>
#include "doublylinkedlist.hpp"
using namespace std;

//implementing stack class for the vfs path function
template <typename E>
class myStack{
public:
	myStack();
	int size() const; //returns the size of the stack
	bool empty() const; //returns if the stack is empty or not
	const E& top(); //returns the top element from the stack
	void push(const E& e); //inserts a new element into the stack
	void pop(); //removes the top element from the stack
private:
	DoublyLinkedList<E> D; //using doublylinked list to implement the stack
	int n;
};

template <typename E>
myStack<E>::myStack()
: D(),n(0){}

template <typename E>
int myStack<E>::size() const{ //returns the size of the stack
	return n;
}

template <typename E>
bool myStack<E>::empty() const{
	return n==0;
}

template <typename E>
const E& myStack<E>::top(){
	if (!empty()){
		return D.front();
	}
}

template <typename E>
void myStack<E>::push(const E& e){
	n=n+1;
	D.addFront(e);
}

template <typename E>
void myStack<E>::pop(){ //removes the top element from the stack
	if (!empty()){
		D.removeFront();
		n=n-1;
	}
}