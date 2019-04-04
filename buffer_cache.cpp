#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<signal.h>


#define MAX_REQUEST 10
#define MAX_THREAD 4

using namespace std;

mutex m1,m2;	// mutex lock

class buffer
{
public:
int block_number,processId;
buffer *hash_next,*hash_previous,*freelist_next,*freelist_previous;
int status;			// status = 0 ( buffer is free ) , status = 1 (buffer is busy) , status = 2 (buffer is marked delayed write)
		
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
		}

		buffer(int block_number, int processId, buffer *hash_next=0, buffer *hash_previous=0, buffer *freelist_next=0, buffer *freelist_previous=0, int status=0)
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
		}

};

class buffer_cache
{
buffer *head,*tail;

public:	
		int countHashBuffer()   
		{	/*
				Objective : Count the number of buffer in Hash Queue
				Input : None
				Return : Number of buffer
			*/

			int count=0;
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
					cout<<"("<<temp->block_number<<","<<temp->status<<")  ";
				}
				else
				{
					do{
						cout<<"("<<temp->block_number<<","<<temp->status<<")  ";
						temp=temp->freelist_next;
					}while(temp!=head);
				}
			}
			
			else
				cout<<"Underflow";

		}

}hashQueue[4], freelist;


void displayHashAndFreeList()
{
	/*
		Objective : Display hash queue and free list
		Input : None
		Return : None
	*/
	
	cout<<"\n\n-----HASH QUEUE------";
	for (int i=0; i<4; i++)
	{
		cout<<"\nHash "<<i<<" : ";
		hashQueue[i].printHash();
	}
	
	cout<<"\n\n-------FREELIST-------";
	cout<<"\nfreelist : ";
	freelist.printFreeList();
}

void blockAcquired(int signum)
{		
		cout<<"\nBlock Acquired Successfully";
		sleep(4);
		signal(SIGUSR1,blockAcquired);
}

void blockReleased(int signum)
{
	cout<<"\nBlock released successfully";
	signal(SIGUSR2,blockReleased);
}


/*void resumeProcess()
{

}*/

buffer* getBlock(int block_number,int pid)
{
	/*
		Objective : Implements the basic getblk algorithm and returns the available buffer (if any)
		            	1. The kernel finds the block on its hash queue, and its buffer is free.
					    2. The kernel cannot find the block on the hash queue, so it allocates a buffer from the free list.
					   	3. The kernel cannot find the block on the hash queue and, in attempting to allocate a buffer from the free list 
					   		(as in scenario 2), finds a buffer on the free list that has been marked "delayed write." The kernel must write the
					      	"delayed write" buffer to disk and allocate another buffer.
						4. The kernel cannot find the block on the hash queue, and the free list of buffers is empty.
					    5. The kernel finds the block on the hash queue, but its buffer is currently busy.

		Input : Block number, ProcessId
		Return : Free Block 
	*/
		
	buffer* allocatedBuffer = NULL;
	while(allocatedBuffer==NULL)
	{
		//signal(SIGUSR1,blo);
		buffer *blockBuffer = hashQueue[block_number%4].searchBuffer(block_number,1);
		if(blockBuffer!=NULL)		// if buffer in hash queue 
		{
			if(blockBuffer->status == 1) // buffer is busy      (scenerio 5)
			{
				cout<<"\nBuffer is busy. Process "<<pid<<" should sleep\n";
				sleep (4);   // sleep(event buffer become free)
				continue;
			}
			freelist.removeSpecificBuffer(block_number,0);     // (scenerio 1)
			allocatedBuffer=blockBuffer;
		}

		else	// block not on hash queue
		{
			buffer* freelistBuffer=freelist.removeBufferFromHeadFreeList();
			if (freelist.isEmpty())			// freelist is empty       (scenerio 4)
			{
				cout<<"\nFreelist is empty. No buffer is available";
				sleep(4);			//sleep(event any buffer become free);
				continue;
			}
			
			else if(freelistBuffer!=NULL)
			{
				if(freelistBuffer->status == 2)  				// buffer marked delayed write     (scenerio 3)
				{
					cout<<"\nAsync write buffer to disk (Delayed Write)";   //asynchronous write buffer to disk
					sleep(4);
					freelistBuffer->status = 0;
					freelist.insertBufferAtHeadFreeList(freelistBuffer);
					continue;
				}
				else			// (scenerio 2) -- Found in free buffer
				{	
					allocatedBuffer = freelistBuffer;
					hashQueue[block_number%4].insertBufferAtTail(freelistBuffer,1);
				}
			}
		}
		return allocatedBuffer;			// return allocated buffer	
	}
}


bool brelse(int block_number,int status)
{
	/*
		Objective : Release the buffer from hash queue
		Input : Block number, status ( to be updated in free list { either free or delayed write } )
		Return : True (buffer is released) or False (buffer not released)
	*/
	
	buffer* release = hashQueue[block_number%4].searchBuffer(block_number,1);   // search buffer on hash queue
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
	/*
		Objective : Updates the content of the buffer with specified values
		Input : Buffer, block number, process id
		Return : None
	*/

	buffer->block_number = block_number;
	buffer->processId = processId;
	buffer->status = 1;    // mark buffer busy
}



void processManager(int processId)
{
	/*
		Objective : Fulfill all the request (process need free buffer to complete its task) of specific Process
		Input : Process Id
		Return : None
	*/

	int request = MAX_REQUEST;
	srand(time(NULL));
	while(request>0)
	{	signal(SIGUSR1,blockAcquired);
		signal(SIGUSR2,blockReleased);
		int blk_num = rand()%20 + 1;

		m2.lock();
		cout<<"\nProcess number  = "<<processId<<"  , requested block number =  "<<blk_num;
		m2.unlock();
		
		m1.lock();		// acquiring lock 		
		
		buffer* block = getBlock(blk_num,processId);
		if(block!=NULL)
		{
			updateBuffer(block,blk_num,processId);
			raise(SIGUSR1);
			displayHashAndFreeList();
			
			m1.unlock();		// releasing lock	
			
			int status = rand()%2;			// randomly selecting status for releasing the buffer
			
			if(status == 1)					
				status=2;	
			
			if(brelse(blk_num,status))
			{
				raise(SIGUSR2);
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
	
	thread t[MAX_THREAD];				// creating thread
	for (int i=0; i<MAX_THREAD; i++)
		t[i] = thread(processManager,i+1);

	for (int i=0; i<MAX_THREAD; i++)
		t[i].join();					// wait for t[i] to finish


	cout<<"\n\nState of free list and hash after execution of process";
	displayHashAndFreeList();
	return 0;
}
