#ifndef _MEMORY_UTILITIES_
#define _MEMORY_UTILITIES_

#include <type_traits>
#include <compiler_portability/cpu_architecture.h>

#define is_power_of_two(n) ( n && ((n & (n - 1)) == 0) ? true : false )

namespace memory
{

template <typename T>
inline T getCPUArchitecture()
{
#ifndef _CPU_ARCHITECTURE_
#error "memory_utilities.h : You have to include compiler_portability/cpu_architecture.h in order to call align_to_target_cpu_architecture"
#endif
    static_assert(std::is_integral<T>::value, "get_cpu_architecture : Only numeric values can be used in specialisation");
#ifdef ARCH_X86
    T architecture = 4;
#elif defined(ARCH_X64)
    T architecture = 8;
#else
#error "memory_utilities.h : Non supported CPU architecture"
#endif
    return architecture;
}

template <typename T>
inline T alignToTargetCPUArchitecture(T input)
{
#ifndef _CPU_ARCHITECTURE_
#error "memory_utilities.h : You have to include compiler_portability/cpu_architecture.h in order to call align_to_target_cpu_architecture"
#endif
    static_assert(std::is_integral<T>::value, "alignToTargetCPUArchitecture : Only numeric values can be used in specialisation");

    T alignment = getCPUArchitecture<T>();
    T alignedNumber = input;

    while(alignedNumber % alignment != 0)
    {
      alignedNumber += 1;
    }

    return alignedNumber;
}

}

#endif