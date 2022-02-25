#ifndef doublylinkedlist_h_included
#define doublylinkedlist_h_included

// doublylinkedlist class prepared in the lab session task
#include <iostream>
#include<string>
#include<sstream>
#include<iomanip>
#include<exception>
using namespace std;


template <typename V>
class Node
{
	private:
		V elem; 
		Node<V>* next; //Link (pointer) to the next Node
		Node<V>* prev; //Link (pointer) to the previous Node
		template <typename T>
		friend class DoublyLinkedList;
		
	public:
		Node(): next(NULL), prev(NULL)
		{}
		Node(V elem) : elem(elem), next(NULL), prev(NULL)
		{}
};
//==============================================================
template <typename T>
class DoublyLinkedList
{
	private:
		Node<T>* head; 	// pointer to the head of List
		Node<T>* tail; // pointer to the tail of List
		int n;     	
	public:
		DoublyLinkedList (); // empty list constructor
		~DoublyLinkedList (); // destructor to clean up all nodes
		bool empty() const; // is list empty?
		int size() const;
		const T& front() const; // get the value (element) from front Node of list
		const T& back() const;  // get the value (element) from last Node of the List 
		void addBefore(Node<T> *ptr, const T& elem);	 //add a new node before ptr
		void addFront(const T& elem); // add a new node to the front of list
		void addBack(const T & elem); //add a new node to the back of the list
		void removeFront(); // remove front node from list
		void removeBack();  // remove last node from list
		void removeElem(T& elem);
		void display() const;	// display all element of the list
		class iterator
		{
		private:
			Node<T> *v;
		public:  //iterator traits
			using difference_type=ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference= T&;
			using iterator_category=bidirectional_iterator_tag;
		public:
			iterator(){};
			iterator (Node<T> *u){
				v=u;
			};
			T& operator*(){
				return v->elem;
			}
			bool operator==(const iterator& p) const{
				return v==p.v;
			}
			bool operator!=(const iterator& p) const{
				return v!=p.v;
			}

			iterator& operator++(){
				if (v->next!=NULL){
					v=v->next;
				}
				return *this;
			}
			iterator& operator--(){
				if (v->prev!=NULL){
					v=v->prev;
				}
				return *this;
			}
		};

		iterator begin(){
			return iterator(head->next);
		}
		iterator end(){
			return iterator(tail);
		}
};
//============================================================
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList ()
{
	head=new Node<T>;
	tail=new Node<T>;
	(*head).next=tail;
	(*tail).prev=head;
}
//============================================================
template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList () // destructor to clean up all nodes
{
	while (!empty()){
		removeFront();
	}
}
//============================================================
template <typename T>
void DoublyLinkedList<T>::removeElem(T& elem)
{
	Node<T>* u= head;
	while (u!=NULL){
		if (u->next->elem==elem){
			Node<T>*v=u->next;
			Node<T>*w=v->next;
			u->next=w;
			w->prev=u;
			delete v;
			break;
		} 
		u=u->next;
	}
}
//============================================================
template <typename T>
bool DoublyLinkedList<T>::empty() const // is list empty?
{
	if ((*head).next==tail and (*tail).prev==head) {
		return true;
	}
	return false;
}
//============================================================
template <typename T>
int DoublyLinkedList<T>::size() const // is list empty?
{
	return n;
}
//============================================================
template <typename T>
const T& DoublyLinkedList<T>::front() const // get front element
{
	if (!empty()){
		return (*head).next->elem;
	}
	else{
		cout<<"The list is empty";
		return (*head).next->elem;
	}
}
//============================================================
template <typename T>
const T& DoublyLinkedList<T>::back() const // get last element
{
	if (!empty()){
		return (*tail).prev->elem;
	}
	else{
		cout<<"The list is empty";
		return(*tail).prev->elem;
	}
}
//============================================================
template <typename T>
void DoublyLinkedList<T>::addBefore(Node<T> *ptr, const T& elem)
{
	Node<T>* u=new Node<T>;
	(*u).elem=elem;
	(*u).next=ptr;
	(*ptr).prev->next=u;
	(*u).prev=(*ptr).prev;
	(*ptr).prev=u;
	n++;

}
//============================================================
template <typename T>
void DoublyLinkedList<T>::addFront(const T& elem) // add to front of list
{
	addBefore(head->next,elem);
}
//============================================================
template <typename T>
void DoublyLinkedList<T>::addBack(const T& elem) // add to Back of the list
{
	addBefore(tail,elem);
}
//============================================================
template <typename T>
void DoublyLinkedList<T>::removeFront() // remove front item from list
{
	if (!empty()){
		Node<T>* u=(*head).next;
		(*head).next=(*u).next;
		(*u).next->prev=head;
		delete u;
		n--;
	}else{
		cout<<"The list is already empty"<<endl;
	}
	
}
//============================================================
template <typename T>
void DoublyLinkedList<T>::removeBack() // remove last item from list
{
	if (!empty()){
		Node<T>* u =(*tail).prev;
		(*tail).prev=(*u).prev;
		(*u).prev->next=tail;
		delete u;
		n--;
	}else{
		cout<<"The list is already empty"<<endl;
	}

}
//============================================================


#endif
