
#include "doublylinkedlist.hpp" //header file for own implementation of doublylinkedlist class
#include "mystack.hpp" //header file for own implementation of stack class
#include "myqueue.hpp" //header file for own implementation of queue class
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>
#include <list>
#include <cwctype>

using namespace std;


class Tree
{

public:
	class iNode  //class iNode has name, size,date, bool var to decide folder or not, parent pointer, pointer to link list containing inode pointers
	{
		public:
			string name;
			int size;
			string dateOfCreation;
			bool Folder;
			iNode *parent;
			DoublyLinkedList<iNode*> *children;
			friend class Tree;
			friend void loadingFile(string fileName, Tree& mytree);
		public:
			iNode();
			iNode(string name, int size=10, string dateOfCreation="", bool Folder=true, iNode *parent=NULL, DoublyLinkedList<iNode*> *children=NULL)
			{
				this->name=name;
				this->size=size;
				this->dateOfCreation=dateOfCreation;
				this->Folder=Folder;
				this->parent=parent;
				this->children=children;
			}
	};
public:
	class Position //nested class for retrieving the position of any arbitrary inode
	{
        public:
            iNode* p;
        public:
            Position(iNode* p)
            {
            	this->p=p; //returns the position to the inode passed as an argument
            }
            iNode* & operator*()
            {
                return p; //dereferences the position and returns the inode pointer
            }
            Position parent() const
            {
            	return Position(p->parent); //returns the position to the parent of the inode
            }
            bool isRoot() const
            {
            	return p->parent == NULL; //returns if the inode is root or note
            }
            bool isExternal() const
            {
            	return p->children==NULL; //returns if the inode is external or not
            }
            bool isInternal() const
            {
            	return !isExternal(); //returns if the inode is internal or not
            }
            friend class Tree;
    };

    typedef std::list<Position> iNodePositionList;

public:

	Tree();
	int size() const; //gives the size of the tree
	bool isEmpty() const; //checks if the tree is empty or not
	Position root() const; // returns the position to the root of the tree
	iNodePositionList preorderpositions() const; //returns the list of positions for all the nodes in the tree
	void addiNode(iNode* temp); //adds a new node to the tree
	string date(); //returns a string as the current system date/time
	string path(Position p); //returns a string as the path of an inode
	void ls(iNode* p); //lists all the directories inside an inode if it's a folder
	void search(string name); //finds any file inode in the tree
	void updateWorkingNode(string userInput); //changes the current directory based on the cd commands
	void movingNode(string name); //moves a file to a certain directory based on specific path or current inode's children
	void removingNode(string name); //removes an inode from the tree
	int nodesize(string name); //returns the size of an inode
	void makeNode(string name); //creates a new node
	void emptybin(); //empties the bin
	void showbin(); //shows the oldest inode from the bin
	void recover(); //recovers the oldest inode from the bin
	void fileSort(); //sorts all the files/folders inside a directory in descending order
	int diskSpace(Position& p); //helper function for finding the size of inode



protected:
	void traversal(iNode* p, iNodePositionList& posl) const; //function to traverse all the nodes of the tree


public:
	iNode* rootiNode; 
	int n;
	iNode* workingiNode;
	string savingPath;
	Queue<iNode*>*myqueue=new Queue<iNode*>(10);
	friend void loadingFile(string fileName, Tree& mytree);

};

Tree::Tree(){  //constructor for the tree class
	rootiNode= new iNode("/", 0, date(),true); //including a root as soon as the tree is constructed; updates the root if file is read
	workingiNode=rootiNode; //default workinginode is root
	n=1; //default size is 1 as it contains root by default
	savingPath="/"; //string to store the path of the previous working directory
}

string Tree::path(Position p){ //implementation of stack to retrieve the path of any inode
	myStack<string> mystack;
	string result;
	if ((*p)->parent==NULL) result="/";
	while ((*p)->parent!=NULL){
		mystack.push((*p)->name);
		p=p.parent();
	}
	while(!mystack.empty()){
		string test=mystack.top();
		result=result+"/"+test;
		mystack.pop();
	}
	return result;
}


void Tree::search(string name){ // finding an inode in the tree and printing out its path
	int sentinel=0;
	string substr;
	stringstream ss(name);
	getline(ss, substr, ' ');
	getline(ss, substr);
	iNodePositionList posl=preorderpositions();
	for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
		if ((*(*it))->name==substr){
			cout<<path(*it);
			cout<<endl;
			sentinel++;
		}
		}
	if (sentinel==0) cout<<"Error:: not found!!!"<<endl; //returning error if the inode is not present

}


int Tree::nodesize(string name){ //caculating the size of an inode in the tree and returning the integer value
	string substr;
	stringstream ss(name);
	getline(ss, substr, ' ');
	getline(ss, substr, ' ');
	if (substr.find('/')==-1){ //size for the inode specified without path
		if (workingiNode->children!=NULL){
			for(typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end(); ++it){
				if ((*it)->name==substr)  return (*it)->size;
			}
		}

	}
	else if (substr.find('/')!=-1){ //size for the inode specified with path
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
			if (path(*it)==substr){
				Tree::Position temp=(*it);
				return diskSpace(temp);
			} 
		}
	}
}


void Tree::fileSort(){ //sorting all the folders and files inside a directory in descending order of their size using bubblesort
	if (workingiNode->children!=NULL){
		int n=workingiNode->children->size();
		for (int i=0; i<n; i++){
			typename DoublyLinkedList<iNode*>::iterator prec=workingiNode->children->begin();
			for (int j=1; j<n-i; j++){
				typename DoublyLinkedList<iNode*>::iterator succ=prec;
				++succ;
				if (((*prec)->size)<((*succ)->size)){
					iNode* tmp=*prec; *prec=*succ; *succ=tmp;
				}
				++prec;
			}

		}
		for(typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end(); ++it){
			if ((*it)->Folder==true) cout<<"dir "<<(*it)->name<<" "<<(*it)->dateOfCreation<<" "<<(*it)->size<<"bytes"<<endl;
			if ((*it)->Folder==false) cout<<"file "<<(*it)->name<<" "<<(*it)->dateOfCreation<<" "<<(*it)->size<<"bytes"<<endl;
		}

	}
	else cout<<"Invalid command!"<<endl; 

}

int Tree:: diskSpace(Position& p){ //for post order traversal to calculate the size of a folder inode
	int s=(*p)->size;
	if((*p)->Folder==true){
		iNodePositionList ch;
		if ((*p)->children!=NULL){
			for (typename DoublyLinkedList<iNode*>::iterator it=(*p)->children->begin(); it!=(*p)->children->end();++it){
			ch.push_back(Position(*it));
			}
		}
		for (Tree::iNodePositionList::iterator q=ch.begin(); q!=ch.end();++q){
			s+=diskSpace(*q);
		}
	}
	return s;
}

void Tree::emptybin(){ //emptying the bin by dequeueing the elements
	while(!myqueue->empty()){
		myqueue->dequeue();
	}
}

void Tree::showbin(){ //showing the oldest inode that was sent to the bin
	if (!myqueue->empty()){
		iNode* oldestiNode=myqueue->front();
		Tree::Position p=Position(oldestiNode);
		string pathofNode;
		pathofNode=path(p);
		cout<<"NextElementToRemove:"<<pathofNode<<"("<<oldestiNode->size<<"bytes,"<<oldestiNode->dateOfCreation<<")"<<endl;
	}
	else cout<<"The bin is empty"<<endl; //if bin doesn't contain any inode
}

void Tree::recover(){ //recovering the oldest inode from the bin back to the tree
	int sentinel=0;
	if(!myqueue->empty()){
		iNode* temp=myqueue->front();
		Tree::Position p=Position(temp->parent);
		string pathofNode;
		pathofNode=path(p);
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
			if (path(*it)==pathofNode){
				iNode* recoverediNode=myqueue->dequeue();
				(*(*it))->children->addBack(recoverediNode);
				sentinel++;
				break;
			}
		}
	} 
 if (sentinel==0) cout<<"Cannot recover the node as the path doesn't exist!!!"<<endl; //error message if the path no more exists
}

void Tree::removingNode(string name){ //removing an inode from the tree
	int sentinel=0, checking=0;
	string first, second;
	stringstream ss(name);
	getline(ss, first, ' ');
	getline(ss, second, ' ');
	if (second.find('/')==-1){ //removing the inode from the current inode
		if (workingiNode->children!=NULL){
			for(typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end(); ++it){
				if ((*it)->name==second){
					iNode* temp= *it;
					workingiNode->children->removeElem((*it));
					myqueue->enqueue(temp);
					sentinel++;
					break;
					}
				}
			}
		}
	else if (second.find('/')!=-1){ //removing inode based on a specified path
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
			if (path(*it)==second){
				iNode* parentoffoundnode=(*(*it))->parent;
				iNode* temp= (*(*it));
				parentoffoundnode->children->removeElem(*(*it));
				myqueue->enqueue(temp);
				sentinel++;
				break;
			}
		}
	}
	if (sentinel==0) cout<<"Error: not found!!!"<<endl;
}


void Tree::movingNode(string name){ //moving an inode within a tree
	int sentinel=0;
	string substr,substrfirst, substrsecond;
	stringstream ss(name);
	getline(ss, substrfirst, ' ');
	getline(ss, substr, ' ');
	string secondParameter=substr;
	getline(ss, substrsecond, ' ');
	if (secondParameter.find('/')==-1 && substrsecond.find('/')==-1){ //moving an inode within the current inode's descendenants
		if (workingiNode->children!=NULL){
			for(typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end(); ++it){
				for (typename DoublyLinkedList<iNode*>::iterator jt=workingiNode->children->begin(); jt!=workingiNode->children->end(); ++jt){
				if ((*it)->name==secondParameter && ((*jt)->name==substrsecond)){
					iNode*temp=new iNode((*it)->name, (*it)->size, (*it)->dateOfCreation, (*it)->Folder, *jt, (*it)->children);
					if ((*jt)->children!=NULL)(*jt)->children->addBack(temp);
					else if ((*jt)->children==NULL){
						DoublyLinkedList<iNode*> *childrenfornode=new DoublyLinkedList<iNode*>;
						childrenfornode->addBack(temp);
						(*jt)->children=childrenfornode;
					}
					workingiNode->children->removeElem(*it);
					sentinel++;
					}
				}
			}
		}		
	}
	else if (secondParameter.find('/')!=-1 && substrsecond.find('/')!=-1){ //moving an inode based on the specified path
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
			for (Tree::iNodePositionList::iterator jt=posl.begin(); jt!=posl.end(); ++jt){
				if (path(*it)==secondParameter && path(*jt)==substrsecond){
					iNode* parentoffoundnode=(*(*it))->parent;
					iNode*temp=new iNode((*(*it))->name, (*(*it))->size, (*(*it))->dateOfCreation, (*(*it))->Folder, (*(*jt)), (*(*it))->children);
					if ((*(*jt))->children!=NULL)(*(*jt))->children->addBack(temp), parentoffoundnode->children->removeElem(*(*it));
					if ((*(*jt))->children==NULL){
						DoublyLinkedList<iNode*> *childrenfornode= new DoublyLinkedList<iNode*>;
						childrenfornode->addBack(temp);
						(*(*jt))->children=childrenfornode;
						parentoffoundnode->children->removeElem(*(*it));
					}
					sentinel++;
				}
			}
		}
	}
 if (sentinel==0) cout<<"Error: include path or check if the destination folder exists!!!"<<endl; //error message if the inode isn't found
}

void Tree::updateWorkingNode(string userInput) //changing the current location of working inode
{
	int sentinel=0;
	string substr,substrfirst;
	stringstream ss(userInput);
	getline(ss, substrfirst, ' ');
	getline(ss, substr, ' ');
	string secondParameter=substr;
	if (substrfirst=="cd" && secondParameter==""){ //setting the current inode to the root
		Tree::Position p= Tree::Position(workingiNode);
		savingPath=path(p);
		workingiNode=rootiNode;
		sentinel++;
	} 
	if(secondParameter==".." && workingiNode->parent!=NULL){ //setting th current inode to the parent of the current inode
		Tree::Position p= Tree::Position(workingiNode);
		savingPath=path(p);
		workingiNode=workingiNode->parent;
		sentinel++;
	} 
	else if (secondParameter==".." && workingiNode->parent==NULL) cout<<"Error: root node reached!"<<endl; //error message if cd .. applied to the root node
	else if (secondParameter.find('.')!=-1) cout<<"Error: it's a file!"<<endl; //error message if trying to go into the file inode
	if (workingiNode->children!=NULL){ //changing directory within current inode's children
	for(typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end(); ++it){ 
		if ((*it)->name==secondParameter && ((*it)->Folder==true)){
			Tree::Position p=Tree::Position(workingiNode);
			savingPath=path(p);
			workingiNode=*it;
			sentinel++;
			break;
			} 
		}
	}
	if (secondParameter.find('/')!=-1){ //changing directory to any specified path
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
				if (path(*it)==secondParameter){
					Tree::Position p=Tree::Position(workingiNode);
					savingPath=path(p);
					workingiNode=(*(*it));
					sentinel++;
				}
			} 
		}
	
	if (secondParameter=="-"){ //changing directory to the previous working directory
		Tree::iNodePositionList posl= preorderpositions();
		for (Tree::iNodePositionList::iterator it=posl.begin(); it!=posl.end(); ++it){
			if (path(*it)==savingPath){
				workingiNode=(*(*it));
				sentinel++;
				break;
			}
		}
	} 
	if (sentinel==0) cout<<"Error: not found!!!"<<endl; //error if the destination inode is not found
}


void Tree::ls(iNode* p){ //listing out all the children of an inode if it's folder
	if (p->Folder==false) cout<<"Error: Not a folder."<<endl;
	if (p->children!=NULL){
		for (typename DoublyLinkedList<iNode*>::iterator it=p->children->begin(); it!=p->children->end();++it){
		if ((*it)->Folder==true) cout<<"dir "<<(*it)->name<<" "<<(*it)->dateOfCreation<<" "<<(*it)->size<<"bytes"<<endl;
		if ((*it)->Folder==false) cout<<"file "<<(*it)->name<<" "<<(*it)->dateOfCreation<<" "<<(*it)->size<<"bytes"<<endl;
		}
	}
}

int Tree::size() const{ //returning size of the tree;
	return n;
}
string Tree:: date() //returning a string for the current system date and time
	{
		time_t now = time(0);
		string mytime = ctime(&now);
		return mytime;
	}

bool Tree::isEmpty() const{ //checking if the tree is empty or not
	return n==0;
}


typename Tree::Position Tree::root() const{ //returning position to the root of the tree
	if (!isEmpty()) return Position(rootiNode);
}

typename Tree::iNodePositionList Tree:: preorderpositions() const{ //returning the doubly linked list of positions of all the nodes in the tree
	iNodePositionList posl;
    traversal(rootiNode, posl);
   	return iNodePositionList(posl);
}

void Tree::addiNode(iNode* temp){ //adding a new node to the tree
	int counter=0;
	DoublyLinkedList<iNode*>listchildren;
	if (workingiNode->children!=NULL && workingiNode->children->empty()==false)
	{
		for (typename DoublyLinkedList<iNode*>::iterator it=workingiNode->children->begin(); it!=workingiNode->children->end();++it){
			if ((*it)->name==temp->name)
			{
				counter++;
				listchildren.addBack(*it);

			} 
		}
	}
	if (counter>0) workingiNode=listchildren.back();
	if (counter==0 && workingiNode->children!=NULL)
	{
	 	workingiNode->children->addBack(temp);
	 	n++;
	 	workingiNode=temp;
	}
	else if (counter==0 && workingiNode->children==NULL)
	{
		DoublyLinkedList<iNode*> *childrenList= new DoublyLinkedList<iNode*>;
		childrenList->addBack(temp);
		workingiNode->children=childrenList;
		n++;
		workingiNode=temp;
	}
}

void Tree::traversal(iNode* p, iNodePositionList& posl) const{ //traversing through all the nodes of the tree in pre order
		posl.push_back(Position(p));
		if (p->Folder==true && p->children!=NULL){
			DoublyLinkedList<iNode*>listOfChildren;
			int n=0;
			for (typename DoublyLinkedList<iNode*>::iterator it=p->children->begin(); it!=p->children->end();++it){
				listOfChildren.addBack(*it);
			}
			for (typename DoublyLinkedList<iNode*>::iterator it=listOfChildren.begin(); it!=listOfChildren.end(); ++it){
				traversal(*it, posl);
			}
		}

	}

Tree mytree;

string date() //returns string as the current system date or time
	{
		time_t now = time(0);
		string mytime = ctime(&now);
		return mytime;
	}

void loadingFile(string fileName, Tree& mytree) //loading the file to build the tree
{
	fstream myFile(fileName, fstream::in);
	if (!myFile.is_open()) //File has not been created!
    {
    	cout<<endl;
    }
    else{
    string line,path,date,size;
		while (myFile.good()){
			string array[3];
			int i=0;
			getline(myFile, line);
			stringstream ss(line);
			while (ss.good()){
		        string substr;
		        getline(ss, substr, ',');
		        array[i]=substr;
		        i++;
		    }
		    path=array[0];
		    size=array[1];
		    date=array[2];
		    if (path=="/") {
		        	typename Tree::iNode*temp= new Tree::iNode("/",stoi(size),date,true);
		        	mytree.rootiNode=temp;
		        }
			DoublyLinkedList<string> mylist;
			stringstream sss(path);
			while(sss.good()){
				string section;
				getline(sss,section, '/');
				if (section!="") mylist.addBack(section);
			}
			for(typename DoublyLinkedList<string>::iterator it=mylist.begin(); it!=mylist.end(); ++it){
				string t=*it;
				if (t.find('.') == -1){
					Tree::iNode* temp= new Tree::iNode(t,stoi(size), date,true, mytree.workingiNode);
					mytree.addiNode(temp);
					}
				else{
					Tree::iNode* temp= new Tree::iNode(t,stoi(size), date,false, mytree.workingiNode);
					mytree.addiNode(temp);
					}
				}
			mytree.workingiNode=mytree.rootiNode;	
	    }
	    myFile.close();
	}
}

void writingToFile(string fileName){ //writing to the file once the user exits

	fstream myFile(fileName,fstream::out);
	Tree::iNodePositionList checkagain=mytree.preorderpositions();
	for (Tree::iNodePositionList::iterator it=checkagain.begin(); it!=checkagain.end(); ++it)
	{
		string path=mytree.path(*it);
		myFile<<path<<","<<(*(*it))->size<<","<<(*(*it))->dateOfCreation<<endl;

	}
	myFile.close();

}
void helpInterface(){

	cout<<"1. help : Prints the menu of commands"<<endl;
	cout<<"2. pwd  : Prints the path of current inode"<<endl;
	cout<<"3. realpath <filename>: Prints the full path of a given filename of a file within the current directory"<<endl;
	cout<<"4. ls : Prints the children of the current inode"<<endl;
	cout<<"5. mkdir <foldername>: Creates a folder under the current folder"<<endl;
	cout<<"6. touch <filename> <size>: Creates a file under the current inode location witht the specified filename, size, and current datetime"<<endl;
	cout<<"7. cd <foldername>: Change current inode to the specified folder"<<endl;
	cout<<"8. cd .. : Change current inode to its parent folder"<<endl;
	cout<<"9. cd : Change current inode to root"<<endl;
	cout<<"10. find <folder/file name> : Returns the path of the file(or the folder)"<<endl;
	cout<<"11. mv <filename> <foldername>: Moves a file located under the current directory to the specified folder"<<endl;
	cout<<"12. rm <filename/foldername> : Removes the specified folder and keeps it in bin"<<endl;
	cout<<"13. size <filename/foldername> : Returns the total size of the folder, including all its subfiles, or the size of file"<<endl;
	cout<<"14. emptybin : Empties the bin"<<endl;
	cout<<"15. showbin : Shows the oldest inode of the bin, including its path"<<endl;
	cout<<"16. recover : Reinstates the oldest inode back from the bin to its original position in the tree"<<endl;
	cout<<"17. exit : Exits the program"<<endl;
}

bool passNameCheck(const string& secondparameter){ //name validating function for the file name
	int sentinel=0;
	char check;
	for (int i=0; i<secondparameter.length(); i++){
		if (!(isalnum(secondparameter.at(i)))){
			check=secondparameter.at(i);
			sentinel++;
		}
	}
	if (sentinel!=1 || check!='.'){
		return false;
	}
	return true;
}

int main()
{
	loadingFile("vfs.dat",mytree); //loading the file if it exists

	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"*************** Welcome to Virtual File System *******************"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;

	helpInterface();
	string user_input,firstparameter, secondparameter,thirdparameter;
	int i;
	do {
		cout<<">";
		getline(cin,user_input);
		stringstream sstr(user_input);
		getline(sstr,firstparameter,' ');
		getline(sstr,secondparameter,' ');
		getline(sstr,thirdparameter);
		if (firstparameter=="help") helpInterface(); //printing out the helping commands
		else if (firstparameter=="pwd"){
			Tree::Position p= Tree::Position(mytree.workingiNode);
			cout<<mytree.path(p)<<endl;
		}
		else if (firstparameter=="realpath"){ //returning the path of the file
			int sentinel=0;
			for (typename DoublyLinkedList<Tree::iNode*>::iterator it=mytree.workingiNode->children->begin(); it!=mytree.workingiNode->children->end();++it){
				if ((*it)->name==secondparameter &&((*it)->Folder!=true))
				{
					Tree::Position p=Tree::Position(*it);
					cout<<mytree.path(p)<<endl;
					sentinel++;
				}
			}
			if (sentinel==0) cout<<"File not found!"<<endl;
		}
		else if (firstparameter=="ls" && secondparameter=="sort"){ //sorting the files and folders of an inode
			mytree.fileSort();
		}
		else if (firstparameter=="ls"){ //listing out the files and folders of an inode
			mytree.ls(mytree.workingiNode);
		}
		else if (firstparameter=="mkdir"){ //making a folder in the tree
			int sentinel=0;
			for(int i = 0; i<secondparameter.length(); i++) {
			    if (!(isalnum(secondparameter.at(i)))){
			    	cout<<"Folder name invalid!"<<endl;
			    	sentinel++;
			    	break;
			    } 
			 }
			if (sentinel==0){ 
				string currentdate=date();
				Tree::iNode* currentnode= mytree.workingiNode;
				Tree::iNode* newnode= new Tree::iNode(secondparameter,10,date(),true,mytree.workingiNode,NULL);
				mytree.addiNode(newnode);
				mytree.workingiNode=currentnode;

			}

		}
		else if (firstparameter=="touch"){ //creating a file in the tree
			int sentinel=0;
			for (char const &ch : thirdparameter) {
		        if (isdigit(ch) == 0)  sentinel++;
    		}
			if (thirdparameter=="" || sentinel!=0){
				cout<<"Error: command not valid!!!";
			} 
			else{
				if (passNameCheck(secondparameter)){
				string currentdate=date();
				Tree::iNode* currentnode= mytree.workingiNode;
				Tree::iNode* newnode= new Tree::iNode(secondparameter,stoi(thirdparameter),date(),false,mytree.workingiNode,NULL);
				mytree.addiNode(newnode);
				mytree.workingiNode=currentnode;
				}
				else cout<<"File name invalid!"<<endl;
			}
		}
		else if (firstparameter=="cd"){ //changing the directory in the filesystem
			mytree.updateWorkingNode(user_input);
		}
		else if (firstparameter=="find"){ //finding a file in the directory
			mytree.search(user_input);
		}
		else if (firstparameter=="mv"){ //moving a file/folder from one place to another in the filesystem
			mytree.movingNode(user_input);
		}
		else if (firstparameter=="rm"){ //removing a file/folder from the tree
			mytree.removingNode(user_input);
		}
		else if (firstparameter=="size"){ //returning the size of the file or folder
			cout<<mytree.nodesize(user_input)<<"bytes"<<endl;
		}
		else if (firstparameter=="emptybin"){ //emptying the bin
			mytree.emptybin();
		}
		else if (firstparameter=="showbin"){ //showing the oldest inode that was deleted
			mytree.showbin();
		}
		else if (firstparameter=="recover"){ //recovering the oldest inode if the path exists
			mytree.recover();
		}
		else if (firstparameter=="exit"){ //exiting from the program
			writingToFile("vfs.dat"); //dumping data before exiting
			break;
		} 
		else{
			cout<<"Invalid command!!!"<<endl;
		}


	} while(firstparameter!="exit");

	return 0;
}









