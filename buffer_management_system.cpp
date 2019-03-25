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
		int countHashBuffer()
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

		buffer* findBuffer(int block_number, int flag)   // if flag =0 -> search into freelist , else search into hashqueue
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
			buffer* temp = findBuffer(block_number,flag);

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

	

}hash[4], freelist;

buffer* get_block(int block_number)//, buffer_cache freelist, buffer_cache hash)
{
	/*
		Algorithm getblk

		Input : block number, freelist, hashlist
		Output : locked buffer that can now be used for block 
	*/
		
		buffer* allocatedBuffer = NULL;
		buffer *blockBuffer = hash[block_number%4].findBuffer(block_number,1);
		if(blockBuffer!=NULL)		// if buffer in hash queue 
		{
			if(blockBuffer->status == 1) // buffer is busy      (scenerio 5)
			{
				cout<<"Buffer is busy. Process should sleep\n";
				usleep (5000);   // sleep(event buffer become free)
				
				/*allocatedBuffer->status = 0;			// status mark free
				buffer* tail=freelist.gettail();
				tail->freelist_next = allocatedBuffer;			// inserting into free list after removing from hash
				allocatedBuffer->freelist_previous=tail;
				tail=tail->freelist_next;
				goto l1;*/

				//freelist.insert_tail(allocatedBuffer->block_number, allocatedBuffer->processId, 0);

				//continue;
			}
			//buffer1->status = 1;
			/*allocatedBuffer->status = 1;			// mark buffer busy
			allocatedBuffer->processId = processId;	// assigning process id
			//hash[block_number%4].insert_tail(block_number,processId,1);  				// mark buffer busy     (senerio 1)
			freelist.deleteBuffer(block_number,0);*/			// remove buffer from freelist
			//return buffer;						
						// return buffer
			else if (blockBuffer -> status == 0)
			{
				freelist.removeSpecificBuffer(block_number,0);
				allocatedBuffer=blockBuffer;
			//return allocatedBuffer;
			}
		}

		else	// block not on hash queue
		{
			buffer* freelistBuffer=freelist.removeBufferFromHeadFreeList();
			if (freelist.isEmpty())			// freelist is empty       (scenerio 4)
			{
				cout<<"Freelist is empty. No buffer is available";
				usleep(5000);			//sleep(event any buffer become free);
				//return NULL;
				//goto l1;
				//continue;
			}
			//freelist.deleteBuffer(block_number,0);    // remove buffer from free list

			//node *buffer2 = freelist.searchBlock(block_number);
			else if(freelistBuffer!=NULL)
			{
			//cout<<"\n\nDeleted successfully from freelist = "<<allocatedBuffer->block_number;
				if(freelistBuffer->status == 2)  				// buffer marked delayed write     (scenerio 3)
				{
					cout<<"Async write buffer to disk (Delayed Write)";//asynchronous write buffer to disk
					usleep(5000);
					freelistBuffer->status = 0;
					freelist.insertBufferAtHeadFreeList(freelistBuffer);
				//allocatedBuffer->status=0;					// mark buffer free after async write
				//continue;
				//goto l1;
				}
				else
				{	if( hash[block_number%4].countHashBuffer() <= 4 )
					{
						allocatedBuffer = freelistBuffer;
					
						hash[block_number%4].insertBufferAtTail(freelistBuffer,1);
			/* senerio 2 -- found a free buffer */
			//buffer *temp=freelist.del_beg();
			//hash[temp->block_number%4].deleteBuffer(temp->block_number);   // remove particular from old hash queue
			//hash[block_number%4].insert_tail(block_number,processId,1);				  // put buffer onto new hash queue
						cout<<"\n\ninserted successfully into hash = "<<freelistBuffer->block_number;
						int n=hash[block_number%4].countHashBuffer();
						cout<<"\n\nCount = "<<n;
					}
					else
					{
						cout<<"\nCannot insert more than 5 buffer in hash";
					}
				}
			//return buffer;											// return buffer
			}
		}
		return allocatedBuffer;			// return allocated buffer	
	//}
}
//buffer_cache hash[4], freelist;

bool brelse(int block_number,int status)
{
	
	buffer* release = hash[block_number%4].findBuffer(block_number,1);
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
		cout<<"\nProcess number  = "<<processId<<"  requested block number =  "<<blk_num;
		buffer* block = get_block(blk_num);
		if(block!=NULL)
		{
			updateBuffer(block,blk_num,processId);
			cout<<"\nworking on block number = "<<blk_num;
			usleep(10000);
			int status = rand()%3;			// either 0,1,2
			if(brelse(blk_num,status))
			{
				cout<<"\nsuccessfully release the buffer";
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

/*void displayHashAndFreeList()
{
	cout<<"\nHash\n";
	cout<<"\nHash 0 : ";
	hash[0].printHash();
	cout<<"\nHash 1 : ";
	hash[1].printHash();
	cout<<"\nHash 2 : ";
	hash[2].printHash();
	cout<<"\nHash 3 : ";
	hash[3].printHash();

	cout<<"\nFreelist\n";
	cout<<"\nfreelist : ";
	freelist.printFreeList();
}
*/
int main()
{
	
	/*int blk_num,process;
	char ch;
	buffer* block;
	srand (time(0));
	for (int i=0; i<20; i++)   // initialising 20 buffer in freelist
	{
				freelist.insertBufferAtTail(new buffer(),0); 
	}

	cout<<"\nHash\n";
		cout<<"\nHash 0 : ";
		hash[0].printHash();
		cout<<"\nHash 1 : ";
		hash[1].printHash();
		cout<<"\nHash 2 : ";
		hash[2].printHash();
		cout<<"\nHash 3 : ";
		hash[3].printHash();

		cout<<"\nFreelist\n";
		cout<<"\nfreelist : ";
		freelist.printFreeList();
	// assuming user will act as process (for testing );
	
	do
	{
		block=NULL;
		cout<<"\nEnter block number = ";
		cin>>blk_num;
		cout<<"Enter processId = ";
		cin>>process;
		block = get_block(blk_num);//,process);  //block request
		cout<<"\nblock return value : "<<block;
		if(block!=NULL)
		{
			updateBuffer(block,blk_num,process);
		}
	
		cout<<"\nAllocated buffer = "<<block;

		cout<<"\nDo you need more block (y/n) = ";
		cin>>ch;

		cout<<"\nHash\n";
		cout<<"\nHash 0 : ";
		hash[0].printHash();
		cout<<"\nHash 1 : ";
		hash[1].printHash();
		cout<<"\nHash 2 : ";
		hash[2].printHash();
		cout<<"\nHash 3 : ";
		hash[3].printHash();

		cout<<"\nFreelist\n";
		cout<<"\nfreelist : ";
		freelist.printFreeList();
	}while(ch=='y');*/

	init();
	cout<<"\nInitially";
	cout<<"\nHash\n";
	cout<<"\nHash 0 : ";
	hash[0].printHash();
	cout<<"\nHash 1 : ";
	hash[1].printHash();
	cout<<"\nHash 2 : ";
	hash[2].printHash();
	cout<<"\nHash 3 : ";
	hash[3].printHash();

	cout<<"\nFreelist\n";
	cout<<"\nfreelist : ";
	freelist.printFreeList();

	thread t(process,1);
	t.join();

	cout<<"\nAfter process 1 done is work";
	cout<<"\nHash\n";
	cout<<"\nHash 0 : ";
	hash[0].printHash();
	cout<<"\nHash 1 : ";
	hash[1].printHash();
	cout<<"\nHash 2 : ";
	hash[2].printHash();
	cout<<"\nHash 3 : ";
	hash[3].printHash();

	cout<<"\nFreelist\n";
	cout<<"\nfreelist : ";
	freelist.printFreeList();


return 0;
}
