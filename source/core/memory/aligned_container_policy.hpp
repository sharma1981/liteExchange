#ifndef _ALIGNED_CONTAINER_POLICY_H_
#define _ALIGNED_CONTAINER_POLICY_H_

#include <core/memory/aligned.hpp>
#include <type_traits>

template<typename T>
class AlignedContainerPolicy
{
    public :
        AlignedContainerPolicy()
        {
            static_assert(std::is_fundamental<T>::value || std::is_base_of<core::Aligned<>, T>::value, "Allowed concurrent container specialisations : \
                                                                                                         Classes derived from core::Aligned, fundamental types");
        }
};

#endif