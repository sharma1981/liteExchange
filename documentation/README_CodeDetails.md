**Thread class , thread affinities & named threads :** The projects uses C++11 thread library for std::mutex and std::condition_variable and also std::lock_guard for scope based lock pattern. However I implemented a thread class using platform specific POSIX, NP POSIX and WindowsAPIs.

The reasons are :

- Ability to set thread stack size , neither Boost nor C++11 threads can do this.

- To provide an OOP way to pin threads to CPU cores, setting thread names and also associating static methods with the thread class such as concurrent::Thread::isHyperThreading.

Regarding setting names for threads , this is how it looks in GDB debugger on Linux :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/gdb_info_thread.png">

On Linux , you can also use ps commands , as in Linux architecture actually threads are also processes named as LWP ( light weight process ) :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/psef.png">

As for Windows side, you can see both names of threads and different thread affinities in Visual Studio debugger thread window :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/thread_names_affinities.png">

Thread class header : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/thread.h

Thread class source : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/thread.cpp

I particularly find setting names for threads very useful for debugging. A note here is that in Linux you can only use maximum 16 characters for thread names. In Windows side, the name support for threads in kernel doesn`t exist , it is a mechanism provided by Windows debuggers.

**Actor/Active object pattern :** Regarding the actor model , you can find information on Herb Sutter`s page : http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/

I find this approach very convenient when designing mutithreaded objects that will run continously. In the class diagram below , you can see that engine`s logger , incoming message dispatcher and the outgoing message processor are actually actors/active objects :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/class_diagram_thread_actor.png">

If you look at their code, you will notice that what you do is basically deriving from concurrent::Actor class and then override virtual run method :

Actor base class : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/actor.h

Logger : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/logger.cpp

Incoming message dispatcher : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/server/server_incoming_message_dispatcher.h

Outgoing message processor : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/server/server_outgoing_message_processor.h

They also pass their name to the base constructor of actor class , therefore their thread will be named threads.

**Thread safe container implementations :** As mentioned in the overview , SPSC lockbuffer queue is used as work queues and they are per worker thread in a thread pool. You can see information about its implementation as below :

https://nativecoding.wordpress.com/2015/06/17/multithreading-lockless-thread-safe-spsc-ring-buffer-queue/

https://nativecoding.wordpress.com/2015/12/20/multithreading-memory-orderings-fine-grained-spsc-lockless-queue-benchmark/

The outgoing message processor uses an unbounded fine grained MPMC queue which you can read about here :

https://nativecoding.wordpress.com/2015/08/30/multithreading-lock-contention-and-fine-grained-vs-coarse-grained-benchmark/

Finally the incoming message dispatcher demultiplexes messages coming from multithreaded FIX server. For the time being it is not lockless , however the main consideration in its implementation is that it provides a method called "flush" rather than pop or dequeue. The reason being is since it will be consumed by only one consumer , there is no need to copy all nodes in the internal linked list but actually we can flush all incoming messages with a basic pointer swap :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/queue_mpsc.hpp

**Task, Thread pool, CPU pinning and hyperthreading considerations  :** The task class packages all kind of tasks and return values. For flexibility it uses C++11 variadic templates and std::function and std::bind , and as for flexibility for the return values it uses both boost::optional and boost::any :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/task.h

One of the main considerations of this project was seeing how we can gain performance by pinning each thread to CPU cores and also experiments affects of hyperthreading. Therefore the thread pool class is able to pin threads to CPU cores and if it is told that to avoid hyperthreading , it will pin threads to cores with only even indexes. ( Odd indexed CPU cores will be logical processors ).

Thread pool is not applying anything like work stealing and work load balancing. The main reason for that , each worker thread is already assigned to a specific security symbol. Therefore work items of worker threads can`t be shared. Here you can see the thread pool`s source :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/thread_pool.cpp

**CPU-CacheLine Aligned Memory allocations :** The main consideration for concurrency performance was avoiding the false sharing issue , which you can read about here :

https://nativecoding.wordpress.com/2015/06/19/multithreading-multicore-programming-and-false-sharing-benchmark/

So the memory layer of the engine provides CPU-cache line aligned allocators in different forms : as STL allocator, as a base class with new/delete overrides that do aligned allocation and frees and also an aligned container policy template class provided for compile time checks using C++11 static_asserts and type traits.

The classes that derived from "aligned" class are tasks, incoming messages and outgoing messages as these will be used thread safe containers heavily. In the picture below , you can see the class diagram :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/class_diagram_aligned.png">

As for the use aligned container policy, the unbounded queue based thread safe containers are derived from it :

<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/class_diagram_aligned_container_policy.png">

Cross platform aligned memory allocations: https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/memory/aligned_memory.cpp

Aligned base class : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/memory/aligned.hpp

Aligned container policy : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/memory/aligned_container_policy.hpp

An aligned STL allocator : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/memory/aligned_allocator.hpp

**Logging in multithreaded environment & thread safe singleton :** As for the design of the logger , it uses a thread safe ring buffer. When you call the log methods, the log message is being pushed on that queue , but they are not directly being written to a log file or std::out. As for logging to file and std::outconsole , the logger uses actor model and its threads dumps all messages at once.

As for the interface exposed by the logger , it uses DLCP ( Double Lock Checking Pattern ) singleton. And it is implemented by using memory fences. Regarding DLCP , see :

http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf

The project also has another singleton type which is called as "Meyers singleton" invented by Scott Meyers. It uses a static local variable. The main advantage is that local static variables in C++11 are thread safe. Therefore there is no need to concern about thread safetiness of the logger.  However using static variables in MSVC is not thread safe : 

https://blogs.msdn.microsoft.com/oldnewthing/20040308-00/?p=40363/

Further more, the order of initialisation of static variables might still be problematic.

DCLP Thread safe singleton : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/design_patterns/singleton_dclp.hpp

Meyers singleton : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/design_patterns/singleton_static.hpp

Logger header : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/logger/logger.h

Logger source : https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/logger/logger.cpp

**Variable synchronisation with atomic variables:** I have previously written about atomics in C++11 :

https://nativecoding.wordpress.com/2015/02/15/transition-to-c11-the-most-important-features/

Basically atomic variables guarantees you safety. In the article above , you can also see about a benchmark of atomic counters vs mutext-protected counters. Note that if you want to specialise atomic template class in C++11, your class should be a POD class. Atomic variable uses in the project :

"Is shutting down" flag in the thread pool as the thread pool itself should be thread safe :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/thread_pool.h

"Is finishing" flag in actor class :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/concurrent/actor.h

2 flags in the logger ( file_logging_enabled and console_loging_enabled ) are atomics :

https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/source/utility/logger.h