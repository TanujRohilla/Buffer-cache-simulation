# Buffer Cache Simulation
> This project implements the Buffer Management System used in UNIX Operating System. It replicates the task kernel in Buffer Management from allocation of buffers to freeing of buffers after use.

# Structure of Buffer
* `block_number` - Disk block number which contains data on disk
* `processId` - Process which has acquired the buffer 
* `Status` - status of buffer (free/busy/delayed write)
* `valid` - buffer contains valid or invalid data
* `hash_next` - Pointer to next buffer on hash queue
* `hash_previous` - Pointer to previous buffer on hash queue
* `freelist_next` - Pointer to next buffer on free list
* `freelist_previous` - Pointer to previous buffer on free list

# Structure of Buffer Pool
The kernel caches data in the buffer pool according to a least recently used algorithm: after it allocates a buffer to a 
disk block, it cannot use the buffer for another block until all other buffers have been used more recently. The kernel
maintains a free list of buffers that preserves the least recently used order. The free list is a doubly linked circular 
list of buffers with a dummy buffer header that marks its beginning and end. Every buffer is put on the free list when 
the system is booted. The kernel takes a buffer from the head of the free list when it wants any free buffer, but it can 
take a buffer from the middle of the free list if it identifies a particular block in the buffer pool. In both cases, it 
removes the buffer from the free list. When the kernel returns a buffer to the buffer pool, it usually attaches the 
buffer to the tail of the free list, occasionally to the head of the free list (for error cases), but never to the 
middle. As the kernel removes buffers from the free list, a buffer with valid data moves closer and closer to head of the 
free list. Hence, the buffers that are closer to the head of the free list have not been used as recently as those that 
are further from the head of the free list.

<p align="center">
  <b>Free List of Buffers</b><br><br>
  <img src="/Images/free_list_of_buffer.png">
</p>

When the kernel accesses a disk block, it searches for a buffer with the appropriate device-block number combination. 
Rather than search the entire buffer pool, it organizes the buffers into separate queues, hashed as a function of the 
device and block number. The kernel links the buffers on a hash queue into a circular, doubly linked list, similar to the 
structure of the free list.

<p align="center">
  <b>Buffers on the Hash Queues</b><br><br>
  <img src="/Images/buffer_hash_queue.png">
</p>

# Algorithm
1. **getblk** - The algorithm for reading and writing disk blocks use the algorithm *getblk* to allocate buffers from the pool
2. **brelse** - When the kernel finishes using the buffer, it release the buffer using *brelse* algorithm  

# Working
1. Multithreading is used for creating the processes.
2. Each thread will call `getblk` algorithm for reading and writing data from `processManager()` function.
3. Mutex Locks are used for synchronisation among multiple threads, so that no other thread modifies the content of buffer pool 
while current thread is executing. All other thread will wait until current thread finishes.
4. Condition Variable is used for signal handling among mutiple threads. `Wait()` and `Signal()` function are used. 
`Wait()` function will be called when any thread is waiting for buffer to become free. Wait causes the current thread to block 
until the condition variable is notified.
The thread will be unblocked when `notify_all()` or `notify_one()` is executed.
`Signal()` is used to wake up all the the waiting process waiting for free buffer or particular buffer.
5. When thread finishes it works. It will call `brelse` algorithm and put the buffer at the end of free list of buffer. 

# How to run
1. Open terminal and execute `g++ -std=c++11 -pthread -o process process.cpp` command.
2. To run the newly created executable file, execute `./process` command.











