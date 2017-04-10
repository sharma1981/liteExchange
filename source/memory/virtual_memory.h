#ifndef _VIRTUAL_MEMORY_
#define _VIRTUAL_MEMORY_

#include <cstddef>

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

namespace memory
{

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
};

}
#endif