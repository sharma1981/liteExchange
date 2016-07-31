#ifndef _LOCK_PROFILER_
#define _LOCK_PROFILER_

#define ENABLE_LOCK_PROFILER 0

#include <string>

#if ENABLE_LOCK_PROFILER
#include <utility/stopwatch.h>
#endif

namespace concurrent
{

class LockProfiler
{
    public :

        void start(const std::string lockName);
        void stop();
        void trace();
    private :
#if ENABLE_LOCK_PROFILER
        int m_lockLastCPUCoreID;
        utility::StopWatch m_lockContentionWatch;
        std::string m_lockName;
#endif

};

#if ENABLE_LOCK_PROFILER
#define DECLARE_LOCK_PROFILER concurrent::LockProfiler _profiler
#define LOCK_PROFILER_START _profiler.start(getName())    // getName is a method of BaseLock class
#define LOCK_PROFILER_END _profiler.stop()
#define LOCK_PROFILER_TRACE _profiler.trace()
#else
#define DECLARE_LOCK_PROFILER
#define LOCK_PROFILER_START
#define LOCK_PROFILER_END
#define LOCK_PROFILER_TRACE
#endif

}//namespace

#endif