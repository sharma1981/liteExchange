#ifndef _THREAD_PROFILER_
#define _THREAD_PROFILER_

#define ENABLE_THREAD_PROFILER 1

#include <string>

#if ENABLE_THREAD_PROFILER
#include <compiler_portability/thread_local.h>
#include <concurrent/thread.h>
#ifdef __linux__
#elif _WIN32
#include <windows.h>
#endif
#endif

namespace concurrent
{

struct ThreadProfilerStorage // Will be stored as TLS
{
#if ENABLE_THREAD_PROFILER
        char m_threadName[MAX_THREAD_NAME_LENGTH+1];
#ifdef __linux__
#elif _WIN32
        HANDLE m_threadPerformanceDataHandle;
#endif
#endif
};

// This class has to be POD as it will be used via TLS
// (thread local storage , see thread_local.h
class ThreadProfiler
{
public:
    ThreadProfiler() = delete;
    ~ThreadProfiler() = delete;
    ThreadProfiler(ThreadProfiler& other) = delete;
    ThreadProfiler& operator=(ThreadProfiler& other) = delete;
    ThreadProfiler(ThreadProfiler&& other) = delete;
    ThreadProfiler& operator=(ThreadProfiler&& other) = delete;

    static void start(ThreadProfilerStorage& tlsData, const std::string& name = "");
    static void stop(ThreadProfilerStorage& tlsData);
    static void trace(ThreadProfilerStorage& tlsData);

private:

    static unsigned long getContextSwitchCount(ThreadProfilerStorage& tlsData);
    static unsigned long getCPUCycleCount(ThreadProfilerStorage& tlsData);
};

#if ENABLE_THREAD_PROFILER
#define DECLARE_THREAD_PROFILER thread_local static concurrent::ThreadProfilerStorage _profiler
#define THREAD_PROFILER_START concurrent::ThreadProfiler::start(_profiler, (getName())) // Get name is a method of thread class
#define THREAD_PROFILER_END concurrent::ThreadProfiler::stop(_profiler)
#define THREAD_PROFILER_TRACE concurrent::ThreadProfiler::trace(_profiler)
#else
#define DECLARE_THREAD_PROFILER
#define THREAD_PROFILER_START
#define THREAD_PROFILER_END
#define THREAD_PROFILER_TRACE
#endif


}//namespace

#endif