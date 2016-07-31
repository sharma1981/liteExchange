#include "thread_profiler.h"

#if ENABLE_THREAD_PROFILER
#include <compiler_portability/ignored_warnings.h>
#include <utility/trace.h>
#include <concurrent/thread.h>
#ifdef __linux__
#include <sys/time.h>
#include <sys/resource.h>
#elif _WIN32
#endif
#endif

namespace concurrent
{

void ThreadProfiler::start(ThreadProfilerStorage& tlsData, const std::string& name)
{
#if ENABLE_THREAD_PROFILER
    // Copy from std string and null terminate
    tlsData.m_threadName[name.copy(tlsData.m_threadName, name.length() - 1)] = 0;
#ifdef __linux__
#elif _WIN32
    auto handle = ::GetCurrentThread();
    ::EnableThreadProfiling(handle, THREAD_PROFILING_FLAG_DISPATCH, 0, &tlsData.m_threadPerformanceDataHandle);
#endif
#endif
}

void ThreadProfiler::stop(ThreadProfilerStorage& tlsData)
{
#if ENABLE_THREAD_PROFILER
#ifdef __linux__
#elif _WIN32
    ::DisableThreadProfiling(tlsData.m_threadPerformanceDataHandle);
#endif
#endif
}

void ThreadProfiler::trace(ThreadProfilerStorage& tlsData)
{
#if ENABLE_THREAD_PROFILER
    if (tlsData.m_threadName != nullptr)
    {
        utility::trace("Thread with name %s on core %d : context switch count = %d, cpu cycles = %d", tlsData.m_threadName, Thread::getCurrentCoreID(), getContextSwitchCount(tlsData), getCPUCycleCount(tlsData));
    }
    else
    {
        utility::trace("Thread on core %d : context switch count = %d, cpu cycles = %d", Thread::getCurrentCoreID(), getContextSwitchCount(tlsData), getCPUCycleCount(tlsData));
    }
#endif
}

unsigned long ThreadProfiler::getContextSwitchCount(ThreadProfilerStorage& tlsData)
{
    unsigned long ret{ 0 };
#if ENABLE_THREAD_PROFILER
#ifdef __linux__
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    // Note we get involuntarily context switches
    ret = usage.ru_nivcsw;
#elif _WIN32
    PERFORMANCE_DATA profilingData;
    ::ReadThreadProfilingData(tlsData.m_threadPerformanceDataHandle, 0x00000001, &profilingData);
    ret = profilingData.ContextSwitchCount;
#endif
#endif
    return ret;
}
unsigned long ThreadProfiler::getCPUCycleCount(ThreadProfilerStorage& tlsData)
{
    unsigned long ret{ 0 };
#if ENABLE_THREAD_PROFILER
#ifdef __linux__
    unsigned int low{0};
    unsigned int high{0};
    __asm__ __volatile__ ("rdtsc" : "=a" (low), "=d" (high));
    ret = ((unsigned long)high << 32) | low;
#elif _WIN32
    PERFORMANCE_DATA profilingData;
    ::ReadThreadProfilingData(tlsData.m_threadPerformanceDataHandle, 0x00000001, &profilingData);
    ret = static_cast<unsigned long>(profilingData.CycleTime);
#endif
#endif
    return ret;
}

} // namespace