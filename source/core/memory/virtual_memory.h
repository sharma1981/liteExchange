#ifndef _VIRTUAL_MEMORY_
#define _VIRTUAL_MEMORY_

#include <cstddef>
#include <new>

#ifdef __linux__
#include <unistd.h>
#include <cstdlib>
#elif _WIN32
#include <windows.h>
#include <malloc.h>
#endif

#include <core/compiler/noexcept.h>
#include "cpu_memory.h"

namespace core
{

#define DEFAULT_VIRTUAL_MEMORY_PAGE_SIZE 4096

class VirtualMemory
{
    public :
        static std::size_t getPageSize()
        {
            std::size_t size = 0;
#ifdef __linux__
            size = (size_t)sysconf(_SC_PAGESIZE);
#elif _WIN32
            SYSTEM_INFO siSysInfo;
            GetSystemInfo(&siSysInfo);
            size = siSysInfo.dwPageSize;
#endif
            return size;
        }

        static std::size_t adjustSizeToPageSize(std::size_t size)
        {
            std::size_t adjustedSize = 0;
            auto pageSize = getPageSize();

            if (size <= pageSize)
            {
                adjustedSize = pageSize;
            }
            else
            {
                int pageCount = size / pageSize;

                if (pageCount * pageSize < size)
                {
                    pageCount += 1;
                }

                adjustedSize = pageCount * pageSize;
            }

            return adjustedSize;
        }

        static void* alignedMalloc(std::size_t size, std::size_t alignment) throw(std::bad_alloc)
        {
            void* ptr{ nullptr };
#ifdef __linux__
            posix_memalign(&ptr, alignment, size);
#elif _WIN32
            ptr = _aligned_malloc(size, alignment);
#endif
            //For just C++11 implementation with std::align
            //See http://en.cppreference.com/w/cpp/core/memory/align

            if (ptr == nullptr)
            {
                throw std::bad_alloc();
            }

            return ptr;
        }

        static void alignedFree(void* ptr) noexcept
        {
#ifdef __linux__
            free(ptr);
#elif _WIN32
            _aligned_free(ptr);
#endif
        }
};

}
#endif