#ifndef _TIMER_
#define _TIMER_

#include <memory>
#include <atomic>
#include <core/stopwatch.h>
#include <core/concurrency/task.h>
#include <core/concurrency/thread.h>
#include <core/concurrency/thread_wait_strategy.h>

namespace core
{

class Timer : public SleepWaitStrategy
{
    public :
        Timer() : m_intervalMicroseconds{0}, m_sleepMicroseconds{0},
        m_threadPtr{nullptr}, m_taskPtr{nullptr}
        {
            m_isStopping.store(false);
        }

        ~Timer() { stop(); }

        void start(Task* handler, long intervalInMicroseconds, long sleepInMicroseconds);
        void stop();

        long getIntervalMicroseconds() const { return m_intervalMicroseconds; }
        long getSleepMicroseconds() const {return m_sleepMicroseconds; }

private :
        long m_intervalMicroseconds;
        long m_sleepMicroseconds;
        std::unique_ptr<core::Thread> m_threadPtr;
        std::unique_ptr<core::Task> m_taskPtr;
        std::atomic<bool> m_isStopping;
        core::StopWatch m_stopWatch;
        static void* workerThreadFunction(void* argument);
};

}

#endif