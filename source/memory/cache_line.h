#ifndef _CACHE_LINE_H_
#define _CACHE_LINE_H_

#include <cstddef>
#include <compiler_portability/alignas.h>

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <cstdlib>
#include <windows.h>
#include <memory>
#endif

#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED alignas(CACHE_LINE_SIZE)

namespace memory
{

inline std::size_t getCacheLineSize()
{
    std::size_t lineSize{ 0 };
#ifdef __linux__
    lineSize = sysconf (_SC_LEVEL1_DCACHE_LINESIZE);
#elif _WIN32
    DWORD bufferSize{ 0 };
    GetLogicalProcessorInformation(0, &bufferSize);

    std::unique_ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION, decltype(free)*> buffer{
        (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(bufferSize), free
    };

    GetLogicalProcessorInformation(&buffer.get()[0], &bufferSize);

    for (DWORD i{ 0 }; i != bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i)
    {
        if (buffer.get()[i].Relationship == RelationCache && buffer.get()[i].Cache.Level == 1)
        {
            lineSize = buffer.get()[i].Cache.LineSize;
            break;
        }
    }

#endif
    return lineSize;
}

}

#endif