#include "lock_profiler.h"

#if ENABLE_LOCK_PROFILER
#include <concurrent/thread.h>
#include <utility/trace.h>
#endif

namespace concurrent
{

void LockProfiler::start(const std::string lockName)
{
#if ENABLE_LOCK_PROFILER
    m_lockName = lockName;
    m_lockContentionWatch.start();
#endif
}

void LockProfiler::stop()
{
#if ENABLE_LOCK_PROFILER
    m_lockLastCPUCoreID = Thread::getCurrentCoreID();
    m_lockContentionWatch.stop();
#endif
}

void LockProfiler::trace()
{
#if ENABLE_LOCK_PROFILER
    utility::trace("Lock ( %s ) contention on core %d : %d milliseconds", m_lockName.c_str(), m_lockLastCPUCoreID, m_lockContentionWatch.getElapsedTimeMilliseconds());
#endif
}

}//namespace