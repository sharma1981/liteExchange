#ifndef _HOOK_GNU_LIBC_H_
#define _HOOK_GNU_LIBC_H_

#include <cstddef>

namespace memory
{

namespace debugging
{
#if __linux__
// http://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html
#include <malloc.h>

inline void initHook (void);
static inline void *gnuLibcMallocHook (std::size_t, const void *);
static inline void gnuLibcFreeHook (void*, const void *);

// Uncomment the next line to set the hooks
void(*__malloc_initialize_hook) (void) = initHook;

inline void initHook(void)
{
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    __malloc_hook = gnuLibcMallocHook;
    __free_hook = gnuLibcFreeHook;
}

static inline void *gnuLibcMallocHook(std::size_t size, const void *caller)
{
    void *result{nullptr};
    // Restore all old hooks
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
	
    // Save underlying hooks
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    // Restore our own hooks
    __malloc_hook = gnuLibcMallocHook;
    __free_hook = gnuLibcFreeHook;
    return result;
}

static inline void gnuLibcFreeHook(void*, const void * caller)
{
    // Restore all old hooks
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    
    
    // Save underlying hooks
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
  
	// Restore our own hooks
    __malloc_hook = gnuLibcMallocHook;
    __free_hook = gnuLibcFreeHook;
}


#endif
}//namespace

}//namespace

#endif