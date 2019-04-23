#ifndef GETBLK 
#define GETBLK


#include "buffer.h"
#include "buffer_cache.h"
#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<signal.h>
#include<condition_variable>
#include<string>
#include<stdio.h>

using namespace std;

bool flag=false;
int i=0;

mutex cv_m,io,m1,dw;	// mutex lock
condition_variable cv;




void waits() 
{
	/*
		Objective : wait causes the current thread to block until the condition variable is notified.  The thread will be unblocked when notify_all() 
					or notify_one() is executed
		Input : None
		Return : None
	*/
    unique_lock<mutex> lk(cv_m);
    cerr << "\nWaiting for buffer to be free \n";
    flag = true;
    cv.wait(lk, []{return i == 1;});
    sleep(3);
    cerr << "\nBuffer is Free. Finished waiting.\n";
}

void signals() 
{
	/*
		Objective : signal all the waiting thread with the help of notify_all()
		Input : None
		Output : None
	*/
    sleep(4);
    {
        lock_guard<mutex> lk(cv_m);
        i = 1;
        cerr << "\nNotifying to All Waiting Processes\n";
    }
    cv.notify_all();
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

void delayedWriteHandler(buffer* freelistBuffer)
{
	dw.lock();

	if(hashQueue[freelistBuffer->block_number%4].searchBuffer(freelistBuffer->block_number,1)==NULL)
		hashQueue[freelistBuffer->block_number%4].insertBufferAtTail(freelistBuffer,1);   // insert marked buffer to the corresponding hash queue
					
	freelistBuffer->status=1;   // mark buffer busy
					
	sleep(4);		

	buffer* temp=hashQueue[freelistBuffer->block_number%4].removeSpecificBuffer(freelistBuffer->block_number,1);   // remove after async write
	temp->status=0;   								// mark free
	freelist.insertBufferAtHeadFreeList(temp);    // add it to the free list	
	dw.unlock();
}

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
	cout<<endl;
}

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
	m1.lock();

	while(allocatedBuffer==NULL)
	{	
		buffer *blockBuffer = hashQueue[block_number%4].searchBuffer(block_number,1);
		if(blockBuffer!=NULL)		// if buffer in hash queue 
		{	
			if(blockBuffer->status == 1) // buffer is busy      (scenerio 5)
			{	
				cout<<"\nBuffer is busy. Process "<<pid<<" should sleep    (scenerio -5 )\n    ";
				m1.unlock();
				waits();
				continue;
			}

			// (scenerio 1)
			cout<<"\nBlock Number "<<block_number<<" is allocated to Process "<<pid<<"   ( scenerio-1)";
			updateBuffer(blockBuffer,block_number,pid);
			freelist.removeSpecificBuffer(block_number,0);     
			displayHashAndFreeList();
			allocatedBuffer=blockBuffer;
			m1.unlock();
		}

		else	// block not on hash queue
		{
			
			if (freelist.isEmpty())			// freelist is empty       (scenerio 4)
			{	
				cout<<"\nFreelist is empty. No buffer is available   (scenerio-4)";
				m1.unlock();
				waits();
				continue;
			}
			
			buffer* freelistBuffer=freelist.removeBufferFromHeadFreeList();
			if(freelistBuffer!=NULL)
			{
				if(freelistBuffer->status == 2)  				// buffer marked delayed write     (scenerio 3)
				{
					cout<<"\nAsync write buffer to disk (Delayed Write)     - (scenerio-3)";   //asynchronous write buffer to disk
					thread t1 = thread(delayedWriteHandler,freelistBuffer);
					t1.detach();
					continue;
				}
				
				// ( Scenerio - 2 )
				cout<<"\nBlock Number "<<block_number<<" is allocated to Process "<<pid<<"   ( scenerio-2)";
				if(hashQueue[freelistBuffer->block_number%4].searchBuffer(freelistBuffer->block_number,1)!=NULL)   // whether buffer is present in hash queue
					hashQueue[freelistBuffer->block_number%4].removeSpecificBuffer(freelistBuffer->block_number,1);
	
				updateBuffer(freelistBuffer,block_number,pid);
				hashQueue[block_number%4].insertBufferAtTail(freelistBuffer,1);
				displayHashAndFreeList();
				allocatedBuffer = freelistBuffer;
				m1.unlock();
			}

		}
		return allocatedBuffer;			// return allocated buffer	
	}
}


bool brelse(int block_number,int status,bool valid)
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
		release->valid = valid;
		return true;
	}
	return false;
}


buffer* bread(int blk_num, int processId)
{
	/*
		Objective : 
		Input : 
		Return :
	*/

	buffer* getBuffer = getBlock(blk_num,processId);
	if(getBuffer->valid == true)
	{	sleep(1);
		return getBuffer;
	}
	cout<<"\nInitiate Disk Read";
	sleep(5);    // initiate disk read
	getBuffer->status = true;
	return getBuffer;
}

buffer* bwrite(int blk_num, int processId)
{	
	/*
		Objective : 
		Input : 
		Return :
	*/
	
	buffer* getBuffer = getBlock(blk_num,processId);
	if(getBuffer->valid == true)
	{	sleep(1);
		return getBuffer;
	}
	cout<<"\nInitiate Disk Write";
	sleep(5);    // initiate disk write
	getBuffer->status = true;
	return getBuffer;	
}


#endif