#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef __linux__
#include <pthread.h>
#elif _WIN32
#include <process.h>
#include <windows.h>
#endif

#include <cstddef>
#include <utility>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>

#include "thread_priority.h"
#include "task.h"

namespace core
{

#define MAX_THREAD_NAME_LENGTH 16
#define THREAD_WAIT_TIMEOUT 5000

// Non copyable & Non movable
class Thread : public boost::noncopyable
{
    public :

        explicit Thread(const std::string& name = "");
        virtual ~Thread();

        void setTask(TaskPtr pTask)
        {
            assert(pTask);
            m_task = std::move(pTask);
        }

        const std::string getThreadName()const {return m_name;}

        void start(std::size_t stackSize=0);
        void join();
        void detach();
        bool isAlive() const;
        int pinToCPUCore(int coreId);
        bool setPriority(ThreadPriority priority);
        ThreadPriority getAssignedPriority() const { return m_priority; }
        int getRealPriority();

        std::string getName() const { return m_name; }

        //STATIC UTILITY METHODS
        static int pinCallingThreadToCPUCore(int coreID = 0);
        static int getCurrentCoreID();
        static unsigned int getNumberOfCores();
        static unsigned long getCurrentThreadID();
        static bool isHyperThreading();

        static inline void yield()
        {
            // C++11 way std::this_thread::yield
#ifdef __linux__
            pthread_yield();
#elif _WIN32
            SwitchToThread();
#endif
        }

        static inline void sleep(unsigned long milliseconds)
        {
            // C++11 way std::this_thread::sleep_for
            // However doesn`t exist in MSVC 2013
            auto iterations = milliseconds / 1000;
            for (unsigned long i(0); i<iterations; i++ )
            {
#ifdef __linux__
                usleep(1000000);
#elif _WIN32
                Sleep(1000);
#endif
            }
        }

    private:

        // Move ctor deletion
        Thread(Thread&& other) = delete;
        // Move assignment operator deletion
        Thread& operator=(Thread&& other) = delete;

        TaskPtr m_task;
        std::string m_name;
        bool m_started;
        bool m_joined;
        ThreadPriority m_priority;

        unsigned long m_threadID;
    #ifdef __linux__
        pthread_attr_t m_threadAttr;
        static int pinToCPUCoreInternal(int coreId, unsigned long threadID);
    #elif _WIN32
        HANDLE m_threadHandle;
        static int pinToCPUCoreInternal(int coreID, HANDLE handle);
    #endif

        bool isJoiningOk() const;

        static void* internalThreadFunction(void* argument);
        void setThreadName();
};

using ThreadPtr = std::unique_ptr<Thread>;

}//namespace
#endif