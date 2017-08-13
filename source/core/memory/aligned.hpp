#ifndef _ALIGNED_H_
#define _ALIGNED_H_

#include <cstddef>
#include <core/compiler/noexcept.h>
#include "aligned_memory.h"

#define IS_POWER_OF_TWO(n) ( n && ((n & (n - 1)) == 0) ? true : false )

namespace core
{

template<std::size_t alignment = CACHE_LINE_SIZE>
class Aligned
{
    public:

        Aligned()
        {
            static_assert(IS_POWER_OF_TWO(alignment), "Template argument must be a power of two.");
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