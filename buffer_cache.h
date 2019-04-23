#ifndef BUFFER_CACHE
#define BUFFER_CACHE

#include "buffer.h"
#include <iostream>


using namespace std;

class buffer_cache
{
buffer *head,*tail;

public:	

		buffer* searchBuffer(int block_number, int flag)  
		{
			/*
				Objective : Check whether a specific Block Buffer is in the free List (Flag = 0 ) or Hash Queue (Flag = 1)
				Input : Block number , Flag 
				Return : Search Block Buffer
			*/
			
			buffer* bufferFound = NULL;

			if (flag==0)	// search buffer in freelist
			{
				if(!isEmpty())
				{
					buffer* temp = head;
					do{
						if(temp->block_number == block_number)
						{
							bufferFound = temp;
							break;
						}
						temp=temp->freelist_next;
					}while(temp!=head);
				}
			}
			
			else if (flag==1)		// search buffer in hash queue
			{
				if(!isEmpty())
				{
					buffer* temp = head;
					do{
						if(temp->block_number == block_number)
						{
							bufferFound = temp;
							break;
						}
						temp=temp->hash_next;
					}while(temp!=head);
				}
			}
			return bufferFound;
		}
		
		
		void insertBufferAtTail(buffer* insertBuffer, int flag)       
		{   
			/*
				Objective : Insert Buffer at Tail of Free List(flag 0) or Hash Queue(flag 1)
				Input : Buffer, Flag
				Return : None
			*/

			if (flag==0)   // insert buffer into freelist
			{
				if(head==NULL && tail==NULL)
				{
					insertBuffer->freelist_next=insertBuffer;
					insertBuffer->freelist_previous=insertBuffer;
					head = insertBuffer;
					tail = insertBuffer;
					
				}

				else
				{
					insertBuffer->freelist_previous=tail;
					insertBuffer->freelist_next=head;
					head->freelist_previous = insertBuffer;
					tail->freelist_next = insertBuffer;
					tail = tail->freelist_next;
				}
			}

			else if (flag==1)		// insert buffer into hash queue
			{
				if(head==NULL && tail==NULL)
				{
					insertBuffer->hash_next=insertBuffer;
					insertBuffer->hash_previous=insertBuffer;
					head = insertBuffer;
					tail = insertBuffer;
				}

				else
				{
					insertBuffer->hash_previous=tail;
					insertBuffer->hash_next=head;
					head->hash_previous = insertBuffer;
					tail->hash_next = insertBuffer;
					tail = tail->hash_next;
				}

			}
		}

		
		void insertBufferAtHeadFreeList(buffer* insertBuffer) 
		{
			/*
				Objective : Insert Buffer at Head of Free List
				Input : Buffer
				Return : None
			*/
		
			if(head==NULL && tail==NULL)
			{
				insertBuffer->freelist_next = insertBuffer;
				insertBuffer->freelist_previous = insertBuffer;
				head = insertBuffer;
				tail = insertBuffer;
			}
			else
			{
				insertBuffer->freelist_next = head;
				insertBuffer->freelist_previous = tail;
				tail->freelist_next = insertBuffer;
				head->freelist_previous = insertBuffer;
				head = insertBuffer;
			}
		
		}

		
		buffer* removeBufferFromHeadFreeList()
		{
			/*
				Objective : Remove Buffer from Head of Free List
				Input : None
				Return : Removed Buffer
			*/

			buffer* temp=NULL;
			if(!isEmpty())
			{
				if(head == head->freelist_next) // only one buffer in freelist
				{
					temp=head;
					head = tail = NULL;
				}
				else
				{
					tail->freelist_next = head->freelist_next;
					head->freelist_next->freelist_previous = head->freelist_previous;
					temp=head;
					head = head->freelist_next;
				}
			}
			return temp;
		}

		
		buffer* removeSpecificBuffer(int block_number,int flag)
		{
			/*
				Objective : Remove a specific Buffer from Free List(flag 0) or Hash Queue(flag 1)
				Input : Block number, Flag
				Return : Removed Buffer
			*/

			buffer* temp = searchBuffer(block_number,flag);

			if(flag==0) // remove buffer from freelist
			{
				if(temp->freelist_next == temp)			// freelist is empty
				{
					head = NULL;
					tail = NULL;
				}
				else
				{
					if(temp == head)				// only one buffer in freelist
					{
						head=head->freelist_next;
					}
					else if(temp == tail)
					{
						tail = tail->freelist_previous;
					}
					temp->freelist_previous->freelist_next = temp->freelist_next;
					temp->freelist_next->freelist_previous = temp->freelist_previous;
				}
			}
			else if (flag==1)	// remove buffer from hash queue
			{
				if(temp->hash_next == temp)			// hash queue is empty
				{
					head = NULL;
					tail = NULL;
				}
				else
				{
					if(temp==head)				// only one element in hash queue
					{
						head=head->hash_next;
					}
					else if(temp==tail)
					{
						tail = tail->hash_previous;
					}
					temp->hash_previous->hash_next = temp->hash_next;
					temp->hash_next->hash_previous = temp->hash_previous;
				}
			}
			return temp;
		}
	

		bool isEmpty()
		{
			/*
				Objective : Check whether list is empty or not
				Input : None
				Return : True (Empty) or False (Non-Empty)
			*/
			if(head==NULL && tail==NULL)
				return true;
			else
				return false;

		}

	
		void printHash()
		{
			/*
				Objective : Print the content of a specific Hash Queue , i.e format -> (block number, process Id)
				Input : None
				Return : None
			*/

			if (!isEmpty())
			{
				buffer* temp = head;
				if(temp->hash_next == temp)			// only one buffer in Hash Queue
				{
					cout<<"("<<temp->block_number<<","<<temp->processId<<")"<<"  ";
				}
				else
				{
					do{
						cout<<"("<<temp->block_number<<","<<temp->processId<<")"<<"  ";
						temp=temp->hash_next;
					}while(temp!=head);
				}
			}
			
			else
				cout<<"Underflow";

		}

		
		void printFreeList()
		{
			/*
				Objective : Print the content of Free List , i.e format -> (block number, status)
				Input : None
				Return : None
			*/

			if (!isEmpty())
			{
				buffer* temp =head;
				if(temp->freelist_next == temp)
				{
					cout<<"("<<temp->block_number<<","<<temp->status<<","<<temp->valid<<")  ";
				}
				else
				{
					do{
						cout<<"("<<temp->block_number<<","<<temp->status<<","<<temp->valid<<")  ";
						temp=temp->freelist_next;
					}while(temp!=head);
				}
			}
			
			else
				cout<<"Underflow";

		}

}hashQueue[4], freelist;

#endif