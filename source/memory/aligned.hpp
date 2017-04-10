#ifndef _ALIGNED_H_
#define _ALIGNED_H_

#include <cstddef>
#include <compiler_portability/noexcept.h>
#include "cpu_cache_line.h"
#include "memory_utilities.h"
#include "aligned_memory.h"

namespace memory
{

template<std::size_t alignment = CACHE_LINE_SIZE>
class Aligned
{
    public:

        Aligned()
        {
            static_assert(is_power_of_two(alignment), "Template argument must be a power of two.");
        }

        /*
            From Herb Sutter`s exceptional C++

            All flavors of operator new() and operator delete() are always static functions, even if
            they're not declared static. Although C++ doesn't force you to say "static" explicitly when
            you declare your own, it's better to do so anyway, because it serves as a reminder to yourself as
            you're writing the code and as a reminder to the next programmer who has to maintain it.
        */

        static void* operator new(std::size_t size) throw(std::bad_alloc)
        {
            void* ret = nullptr;

            ret = alignedMalloc(size, alignment);

            return ret;
        }

        static void operator delete(void* ptr) noexcept
        {
            alignedFree(ptr);
        }
};

} //namespace

#endif