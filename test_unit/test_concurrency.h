#include<core/concurrency/task.h>
#include<core/concurrency/actor.h>
#include<core/concurrency/thread.h>
#include<core/concurrency/spinlock.hpp>
#include<core/concurrency/thread_pool.h>
#include<core/concurrency/ring_buffer_spsc_lockfree.hpp>
#include<core/concurrency/queue_mpmc.hpp>
#include<core/concurrency/queue_mpsc.hpp>
#include<core/concurrency/ring_buffer_mpmc.hpp>

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <vector>
#include <thread>
using namespace std;
using namespace core;

class worker
{
    public :
        void* run(void* arg)
        {
            int* p_val = static_cast<int *>(arg);
            ++(*p_val);
            return nullptr;
        }
};

class fred : public core::Actor
{
    int foo = 0;
    public:

        void *run() override
        {
            foo++;
            cout << "fred" << endl;
            return nullptr;
        }

        int getFoo() const { return foo; }
};

TEST(Concurrent, Thread)
{
    worker w;
    int testVal = 665;

    // Building thread
    TaskPtr task(new Task(&worker::run, &w, static_cast<void *>(&testVal)));
    core::Thread t1;
    t1.setTask(std::move(task));

    t1.start();
    t1.join();

    EXPECT_EQ(666, testVal);
}

TEST(Concurrent, Spinlock)
{
    int sum = 10;
    int actual = 0;
    std::vector<std::thread> threads;
    core::SpinLock sharedLock;

    for (int i {0}; i < sum; i++)
    {
        threads.push_back(std::thread([&](){
                                                sharedLock.lock();
                                                actual++;
                                                sharedLock.unlock();
                                            }));
    }

    for (auto& elem : threads)
    {
        elem.join();
    }

    EXPECT_EQ(sum, actual);
}

TEST(Concurrent, Actor)
{
    fred f;
    f.start();
    f.join();
    int testVal = f.getFoo();
    EXPECT_EQ(1, testVal);
}

TEST(Concurrent, RingBufferSPSCLockFree)
{
    core::RingBufferSPSCLockFree<int> queue(19);
    std::vector<std::thread> threads;
    vector<int> testVector = {4,5,7,2};
    int sum = 0;

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ queue.push(i); }));
        sum += i;
    }

    int testSum = 0;

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){    int n = 0;
                                                while(true)
                                                {
                                                    if(queue.tryPop(&n) )
                                                    {
                                                        break;
                                                    }
                                                }
                                                cout << n << endl; testSum += n;
                                            }));
    }

    for (auto& elem : threads)
    {
        elem.join();
    }

    EXPECT_EQ(sum, testSum);
}


TEST(Concurrent, QueueMPMC)
{
    core::QueueMPMC<int> mqueue;
    std::vector<std::thread> threads;
    vector<int> testVector = { 4, 5, 7, 2 };
    int sum = 0;
    int testSum = 0;

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ mqueue.enqueue(i); }));
        sum += i;
    }

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ int i = 0; mqueue.dequeue(&i); testSum += i; }));
    }

    for (auto& elem : threads)
    {
        elem.join();
    }

    EXPECT_EQ(sum, testSum);
}

TEST(Concurrent, QueueMPSC)
{
    core::QueueMPSC<int> mqueue;
    std::vector<std::thread> threads;
    vector<int> testVector = { 4, 5, 7, 2 };
    int sum = 0;
    int testSum = 0;

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ mqueue.push(i); }));
        sum += i;
    }

    std::thread readThread([&]()
                    {
                        core::QueueMPSC<int>::QueueMPSCNode* node = nullptr;
                        node = mqueue.flush();
                        while (node)
                        {
                            testSum += node->m_data;
                            node = node->m_next;
                        }

                    });

    for (auto& elem : threads)
    {
        elem.join();
    }

    readThread.join();

    EXPECT_EQ(sum, testSum);
}

TEST(Concurrent, RingBufferMPMC)
{
    core::RingBufferMPMC<int> ringBuffer(10);
    std::vector<std::thread> threads;
    vector<int> testVector = { 4, 5, 7, 2};
    int sum = 0;
    int testSum = 0;

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ringBuffer.push(i); }));
        sum += i;
    }

    for (auto i : testVector)
    {
        threads.push_back(std::thread([&](){ int n = ringBuffer.pop(); testSum += n; }));
    }

    for (auto& elem : threads)
    {
        elem.join();
    }

    EXPECT_EQ(sum, testSum);
}

struct ThreadPoolJobArgs
{
    int* array;
    int index;
};

class ThreadPoolJob
{
    public:
        void* run(ThreadPoolJobArgs arg)
        {
            arg.array[arg.index] = 1;
            return nullptr;
        }
};

TEST(Concurrent, ThreadPool)
{
    vector<string> threadNames = { "a", "b", "c", "d" };

    core::ThreadPoolArguments args;
    args.m_hyperThreading = true;
    args.m_pinThreadsToCores = true;
    args.m_threadNames = threadNames;
    args.m_workQueueSizePerThread = 1024;
    core::ThreadPool pool;

    pool.initialise(args);

    int* workArray = new int[ threadNames.size() ];
    ThreadPoolJob job;

    ThreadPoolJobArgs job_args;
    job_args.array = workArray;

    int expectedSum = 0;

    for (size_t i = 0; i < threadNames.size(); i++)
    {
        job_args.index = i;
        expectedSum++;
        core::Task task(&ThreadPoolJob::run, &job, job_args);
        pool.submitTask(task, i);
        core::Thread::sleep(1000000); // Let`s wait long enough , 1 second , to make sure that all thread pool jobs invoked
    }

    pool.shutdown();

    int actualSum = 0;

    std::for_each(workArray, workArray + threadNames.size(), [&] (int val) { actualSum+= val;});

    delete[] workArray;

    EXPECT_EQ(actualSum, expectedSum);
}