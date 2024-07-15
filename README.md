# khang_exchange

## Targets:
1. Set up Websocket echo server: Done
	a. Add json parser: Done
	
	b. Handle multiple connections: Done
	
	c. Handle basic thread + IO-thread: Done
	
	d. Update myServer to include the IO-thread + Consider event driven: Done
	
	e. Test basic logger with IO-thread: Done
	
	f. Set up basic single-threaded event loop on main thread with asio::io_service: Done 
	
	g. Switch to thread-pool: Done

2. Add send/ receive orders features: Done

User account: {userId: string, tickersAmount: map<string, int>}

Order structure: {userId: string, orderId: string, tickers: string, price: int, amount: int, sell: boolean, submitTime: time_t}

Order book (2 side: Buy - Sell): For each ticker, maintains an ordered set by price and 
time of submission for O(log n) access (can further optimized by grouping the set by prices
but still O(log n) worst case regardless).


3. Add matching orders feature: Done (for a single ticker for now)

4. Add efficient removal of orders: O(1) armortized with cached location of order in
the balanced tree.

5. Add logger: Add buffer to boost performance

6. Host on Oracle servers

7. Add display features 

8. Add bots to stimulate market

## Run the server
Guides for Visual Studio 2022 (with vcpkg)

1. Click "Generate" for CMake
2. Choose "khang_exchange.cpp" as target, then click run (auto build)


## References:
Basic websocket tutorial: https://github.com/club-plus-plus/websocket-server-workshop/blob/master/Part4.md

## Notes:
Personal notes on random topics read while building projects.

### Websocket basics:
Include as comments for websocketpp.
Protocol: TBC


### Multi-threading keywords:
https://www.backblaze.com/blog/whats-the-diff-programs-processes-and-threads/
1. Stack: Scoped storage for resources. Flushed when the function returns.
2. Heap: Dynamic storage that allows manual allocation and requires manual deallocation
(without garbage collection)
3. Process: Program loaded into memory, with all resources and separated memory.
4. Thread: "Lightweight" process, but with shared heap and independent stack.

The threads might be run parallely (with multi-core) or concurrently (single core):
1. Parallelism: True simultaneous running of threads.
2. Concurrency: Alternating between threads to simulate parallelism.

Async process key components:
1. Event loop: Process to wait/ listen to event providers, and dispatch the event to correct handlers. 
2. Message queue: Queue used to store incoming messages, which will be dequeued and processed by the event loop.
3. Thread pool: A pool (group) of threads that can be reused to boost efficiency by avoiding overhead of creating new threads.

Issues with shared resources in heap:
1. Deadlock: Where 2 or more threads are waiting for each other to release resources.
2. Livelock: Where multiple threads are able to execute but the system as a whole doesn't proceed.
3. Starvation: Low-priority resources are kept in waiting forever due to the high-priority ones keep coming.

Synchronization - Managing shared resources between threads:

For lock and condition variable: https://web.stanford.edu/~ouster/cs111-spring21/lectures/locks/#:~:text=Lock%3A%20an%20object%20that%20can,until%20the%20lock%20is%20free.
For semaphore: https://blog.feabhas.com/2009/09/mutex-vs-semaphores-%E2%80%93-part-1-semaphores/

1. Locks: Object that can be owned by a SINGLE thread at a time (std::mutex)
2. Mutex (std::mutex mx): Simple lock: Need to lock resource before usage.
The mutex lock stops all threads trying to acquire the lock until the resource is unlocked.
Used with unique_lock to allow automatic deallocation when out of scope.

3. Semaphore (std::counting_semaphore, std::binary_semaphore): Lock combined with a counter.
If the counter is positive, lock process decrements the count and allows access; otherwise, wait until the resource is unlocked.
Unlock process increments the counter.

4. Condition variable (sdt::condition_variable): Used to wait until desirable state change is reached (eg: count reached 10).
Used with while loop to repeatedly wait until the correct condition.
3 main methods:
	i. wait(lock: mutex): Automatically released lock, put thread to sleep until the thread is notified.
	When wake up, acquire lock automatically until return.
	ii. notify_one(): Wake up one sleeping thread.
	iii. notify_all(): Wake up all sleeping threads.
