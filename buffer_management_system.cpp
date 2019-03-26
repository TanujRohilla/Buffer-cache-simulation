#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<thread>

using namespace std;

class buffer
{
public:
int block_number,processId;
buffer *hash_next,*hash_previous,*freelist_next,*freelist_previous;
int status;			// status = 0 ( buffer is free ) , status = 1 (buffer is busy) , status = 2 (buffer is marked delayed write)

//public:		
		buffer()
		{
			block_number = 0;
			processId = 0;
			hash_previous = 0;
			hash_next = 0;
			freelist_previous = 0;
			freelist_next = 0;
			status = 0;
		}

		buffer(int block_number, int processId, buffer *hash_next=0, buffer *hash_previous=0, buffer *freelist_next=0, buffer *freelist_previous=0, int status=0)
		{
			this->block_number = block_number;
			this->processId = processId;
			this->hash_next = hash_next;	
			this->hash_previous = hash_previous;
			this->freelist_next = freelist_next;
			this->freelist_previous = freelist_previous;
			this->status = status;
		}

		void display()
		{
			cout<<"\nBlock number = "<<block_number;
			cout<<"\nProcess Id = "<<processId;
			cout<<"\nStatus = ";
			if(status==0)
				cout<<"Buffer is free";
			else if(status==1)
				cout<<"Buffer is busy";
			else if(status==-1)
				cout<<"Delayed Write";
		}
};

class buffer_cache
{
buffer *head,*tail;

public:	
		int countHashBuffer()   // count the number of buffer in hash queue
		{	int count=0;
			if(!isEmpty())
			{
				buffer* temp=head; 

					do{
						temp=temp->hash_next;
						count++;
					}while(temp!=head);

					return count;
				

			}
			else
				return count;

		}

		buffer* searchBuffer(int block_number, int flag)   // if flag =0 -> search into freelist , else search into hashqueue
		{
			buffer* bufferFound = NULL;

			if (flag==0)	// find in freelist
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
			else if (flag==1)		// find in hash
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
		

		void insertBufferAtTail(buffer* insertBuffer, int flag)     // if flag=0 -> insert into freelist , else if flag=1 -> insert into hashlist  
		{   
			if (flag==0)   // insert into freelist
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

			else if (flag==1)		// insert into hash 
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

		void insertBufferAtHeadFreeList(buffer* insertBuffer) // if flag=0 -> insert into freelist, else insert into hashlist
		{
		
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
			buffer* temp;
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
			buffer* temp = searchBuffer(block_number,flag);

			if(flag==0) // remove buffer from freelist
			{
				if(temp->freelist_next == temp)
				{
					head = NULL;
					tail = NULL;
				}
				else
				{
					if(temp == head)
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
			else if (flag==1)	// remove buffer from hash
			{
				if(temp->hash_next == temp)
				{
					head = NULL;
					tail = NULL;
				}
				else
				{
					if(temp==head)
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
			if(head==NULL && tail==NULL)
				return true;
			else
				return false;

	}

	

	void printHash()
	{

			if (!isEmpty())
			{
				buffer* temp =head;
				if(temp->hash_next == temp)
				{
					cout<<temp->block_number;
				}
				else
				{
					do{
						cout<<temp->block_number<<" ";
						temp=temp->hash_next;
					}while(temp!=head);
				}
			}
			
			else
				cout<<"Underflow";

	}

	void printFreeList()
	{
			if (!isEmpty())
			{
				buffer* temp =head;
				if(temp->freelist_next == temp)
				{
					cout<<temp->block_number;
				}
				else
				{
					do{
						cout<<temp->block_number<<" ";
						temp=temp->freelist_next;
					}while(temp!=head);
				}
			}
			
			else
				cout<<"Underflow";

	}

	

}hashQueue[4], freelist;

buffer* getBlock(int block_number)
{
	/*
		Algorithm getblk

		Input : block number, freelist, hashlist
		Output : locked buffer that can now be used for block 
	*/
		
		buffer* allocatedBuffer = NULL;
		buffer *blockBuffer = hashQueue[block_number%4].searchBuffer(block_number,1);
		if(blockBuffer!=NULL)		// if buffer in hash queue 
		{
			if(blockBuffer->status == 1) // buffer is busy      (scenerio 5)
			{
				cout<<"\nBuffer is busy. Process should sleep\n";
				usleep (5000);   // sleep(event buffer become free)
			}
			
			else if (blockBuffer -> status == 0)
			{
				freelist.removeSpecificBuffer(block_number,0);
				allocatedBuffer=blockBuffer;
			}
		}

		else	// block not on hash queue
		{
			buffer* freelistBuffer=freelist.removeBufferFromHeadFreeList();
			if (freelist.isEmpty())			// freelist is empty       (scenerio 4)
			{
				cout<<"\nFreelist is empty. No buffer is available";
				usleep(5000);			//sleep(event any buffer become free);
			}
			
			else if(freelistBuffer!=NULL)
			{
				if(freelistBuffer->status == 2)  				// buffer marked delayed write     (scenerio 3)
				{
					cout<<"\nAsync write buffer to disk (Delayed Write)";//asynchronous write buffer to disk
					usleep(5000);
					freelistBuffer->status = 0;
					freelist.insertBufferAtHeadFreeList(freelistBuffer);
				}
				else
				{	if( hashQueue[block_number%4].countHashBuffer() <= 4 )
					{
						allocatedBuffer = freelistBuffer;
						hashQueue[block_number%4].insertBufferAtTail(freelistBuffer,1);
					}
					else
					{
						cout<<"\nCannot insert more than 5 buffer in hash";
					}
				}
			}
		}
		return allocatedBuffer;			// return allocated buffer	
	
}

bool brelse(int block_number,int status)
{
	
	buffer* release = hashQueue[block_number%4].searchBuffer(block_number,1);
	if(release!=NULL)
	{
		release->status = status;
		freelist.insertBufferAtTail(release,0);
		return true;
	}
	return false;
}

void updateBuffer(buffer* buffer,int block_number,int processId)
{
	buffer->block_number = block_number;
	buffer->processId = processId;
	buffer->status = 1;    // buffer mark busy;
}

void process(int processId)
{
	int request = 10;
	srand(time(NULL));
	while(request>0)
	{
		int blk_num = rand()%20 + 1;
		cout<<"\nProcess number  = "<<processId<<"  , requested block number =  "<<blk_num;
		buffer* block = getBlock(blk_num);
		if(block!=NULL)
		{
			updateBuffer(block,blk_num,processId);
			cout<<"\nWorking on block number = "<<blk_num;
			usleep(10000);
			int status = rand()%3;			// either 0,1,2	
			if(brelse(blk_num,status))
			{
				cout<<"\nSuccessfully release the buffer";
			}
			else
			{
				cout<<"\nBuffer is not released due to some error";
			}			
		}
		else
		{
			cout<<"\nUnable to access the requested block";
		}
		request--;
	}
}

void displayHashAndFreeList()
{
	cout<<"\n\n-----HASH QUEUE------";
	cout<<"\nhash 0 : ";
	hashQueue[0].printHash();
	cout<<"\nhash 1 : ";
	hashQueue[1].printHash();
	cout<<"\nhash 2 : ";
	hashQueue[2].printHash();
	cout<<"\nhash 3 : ";
	hashQueue[3].printHash();

	cout<<"\n\n-------FREELIST-------";
	cout<<"\nfreelist : ";
	freelist.printFreeList();
}

int main()
{
	// inilizatilizing free list
	srand (time(0));
	for (int i=0; i<20; i++)   // initialising 20 buffer in freelist
	{
				freelist.insertBufferAtTail(new buffer(),0); 
	}

	cout<<"\nInitially";
	displayHashAndFreeList();
	
	thread t1(process,1);
	thread t2(process,2);
	thread t3(process,3);
	thread t4(process,4);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	cout<<"\nState of free list and hash after execution of process";
	displayHashAndFreeList();
	return 0;
}
