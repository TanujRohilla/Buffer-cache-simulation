#ifndef BUFFER 
#define BUFFER

#include<iostream>


using namespace std;

class buffer
{
public:
int block_number,processId;
buffer *hash_next,*hash_previous,*freelist_next,*freelist_previous;
int status;			// status = 0 ( buffer is free ) , status = 1 (buffer is busy) , status = 2 (buffer is marked delayed write)
bool valid;
		
		buffer()
		{
			/*
				Objective : Default Constructor for the Buffer
				Input : None
				Return : None
			*/

			block_number = 0;
			processId = 0;
			hash_previous = 0;
			hash_next = 0;
			freelist_previous = 0;
			freelist_next = 0;
			status = 0;
			valid = false;
		}

		buffer(int block_number, int processId, buffer *hash_next=0, buffer *hash_previous=0, buffer *freelist_next=0, buffer *freelist_previous=0, int status=0, bool valid=false)
		{
			/*
				Objective : Parameterized Constructor to initialize Variables for the Buffer
				Input : None
				Return : None 
			*/

			this->block_number = block_number;
			this->processId = processId;
			this->hash_next = hash_next;	
			this->hash_previous = hash_previous;
			this->freelist_next = freelist_next;
			this->freelist_previous = freelist_previous;
			this->status = status;
			this->valid = valid;
		}

		void display()
		{
			/*
				Objective : Function to print Information about the Current Buffer
				Input : None
				Return : None
			*/
			
			cout<<"\nBlock number = "<<block_number;
			cout<<"\nProcess Id = "<<processId;
			cout<<"\nStatus = ";
			if(status==0)
				cout<<"Buffer is free";
			else if(status==1)
				cout<<"Buffer is busy";
			else if(status==-1)
				cout<<"Delayed Write";
			cout<<"\n Data Valid = "<<valid;
		}

};

#endif
