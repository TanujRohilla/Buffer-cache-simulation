#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<signal.h>
#include<stdio.h>
#include "buffer.h"
#include "buffer_cache.h"
#include "getblk.h"

#define MAX_REQUEST 3
#define MAX_THREAD 3	
#define MAX_BUFFER 2


using namespace std;


void processManager(int processId)
{
	/*
		Objective : Fulfill all the request (process need free buffer to complete its task) of specific Process
		Input : Process Id
		Return : None
	*/

	int request = MAX_REQUEST;
	srand(time(0));
	while(request>0)
	{	
		int blk_num,rw;
		string choice;
		buffer* block;
		io.lock();
		blk_num = rand()%20 + 1;
		cout<<"\nProcess number  = "<<processId<<"  , requested block number =  "<<blk_num;

		rw = rand()%2;
		
		if (rw == 0)
		{
			cout<<"\nProcess = "<<processId<<" wants to READ from blk number = "<<blk_num;
			choice="read";
		}
		else
		{
			cout<<"\nProcess = "<<processId<<" wants to WRITE from blk number = "<<blk_num;
			choice="write";	
		}
		
		io.unlock();

		if(choice.compare("read")==0)
			block = bread(blk_num,processId);
		
		else if(choice.compare("write")==0)
			block = bwrite(blk_num,processId);
		
		
		
		if(block!=NULL)
		{
			//sleep(6);
			int status = rand()%2;			// randomly selecting status for releasing the buffer
			int i = rand()%2;
			bool valid_bit;
			
			if(status == 1)					
				status=2;	

			if(i == 1)
				valid_bit = true;
			else if (i  == 0)
				valid_bit = false;
			
			if(brelse(blk_num,status,valid_bit))     // releasing the buffer
			{
				cout<<"\nSuccessfully release the buffer "<<blk_num<<" by Process "<<processId;
				if (flag == true)  
				{
					signals();    // sending signal to all the waiting process
					flag=false;
				}
			}
					
		}
		request--;
	}
}

int main()
{
	// inilizatilizing free list
	srand (time(0));
	for (int i=0; i<MAX_BUFFER; i++)   // initialising n buffer in freelist
	{
				freelist.insertBufferAtTail(new buffer(),0); 
	}
	io.lock();
	cout<<"\nInitially";
	displayHashAndFreeList();
	io.unlock();
	
	thread t[MAX_THREAD];				// creating thread
	for (int i=0; i<MAX_THREAD; i++)
		t[i] = thread(processManager,i+1);

	for (int i=0; i<MAX_THREAD; i++)
		t[i].join();					// wait for t[i] to finish


	cout<<"\n\nState of free list and hash after execution of process";
	displayHashAndFreeList();
	return 0;
}
