#include<iostream>
using namespace std;

//implementing queue using array as we need a fixed size of bin 
template <typename T>
class Queue
{
	private:
		T *array;
		int N;	//Array Size (Capacity)
		int r;	//index where a new element will be added
	public:
		Queue(int N);
		~Queue();
		void enqueue(T elem); 
		T dequeue();
		bool empty();
		bool isFull();
		T front();
};

//========================================================
//Constructor
template<typename T> Queue<T>::Queue(int N): N(N)
{
	this->array = new T[N];
	this->r=0;
}
template<typename T> Queue<T>::~Queue()
{
	delete[] array;
}
template<typename T> 
void Queue<T>::enqueue(T elem) //inserting element into the queue
{
	if (!isFull()){         
		array[r]=elem;
		r+=1;                
	}
	else{
		dequeue();
		array[r]=elem;
	}
}
template<typename T> 
T Queue<T>::dequeue() //removing element from the queue
{
	T temp=array[0];
	for (int i=0; i<r-1; i++){     
		array[i]=array[i+1];
	}
	r-=1;                    
	return temp;

}
template<typename T>  //checking if the queue is empty or not
bool Queue<T>::empty()
{
	return r==0;            
}
template<typename T> 
bool Queue<T>::isFull() 
{
	return r==N;           
}
template<typename T>
T Queue<T>::front() //retrieving the front element from the queue
{
	if (!empty()) return array[0];
}
//=====================================================