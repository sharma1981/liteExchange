#include "timer.h"
#include <utility>
using namespace std;

namespace core
{

void Timer::start(Task* handler, long intervalInMicroseconds, long sleepInMicroseconds)
{
    m_intervalMicroseconds = intervalInMicroseconds;
    m_sleepMicroseconds = sleepInMicroseconds;

    m_taskPtr.reset(handler);

    TaskPtr task( new Task(&Timer::workerThreadFunction,this));
    Thread* thread = new Thread("Timer");
    thread->setTask(std::move(task));
    m_threadPtr.reset(thread);
    m_threadPtr->start();
}

void* Timer::workerThreadFunction(void* argument)
{
    Timer* timer = static_cast<Timer*>(argument);
    timer->m_stopWatch.start();

    while(true)
    {
        if( timer->m_isStopping.load() == true )
        {
            break;
        }

        timer->m_stopWatch.stop();
        auto elapsedTime = timer->m_stopWatch.getElapsedTimeMicroseconds();

        if( elapsedTime >= timer->m_intervalMicroseconds)
        {
            timer->m_taskPtr->execute();
            timer->m_stopWatch.stop();
            timer->m_stopWatch.start();
        }

        timer->applyWaitStrategy(timer->m_sleepMicroseconds);
    }

    return nullptr;
}

void Timer::stop()
{
    m_isStopping.store(true);
    if( m_threadPtr.get())
    {
        m_threadPtr->join();
    }
}

}