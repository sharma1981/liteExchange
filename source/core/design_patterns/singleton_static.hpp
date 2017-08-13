#ifndef _SINGLETON_STATIC_H_
#define _SINGLETON_STATIC_H_

#include <boost/noncopyable.hpp>

/*
    Scott Meyers` singleton
    In C++11 statics are initialized in a thread safe way

    Note that , the order of initialisation for statics in different translation units
    is undefined , so this could be problematic if more singletons used in the project

    Note about MSVC : In MSVC using static are not thread safe :
    https://blogs.msdn.microsoft.com/oldnewthing/20040308-00/?p=40363/
*/

namespace core
{

template<typename T>
class SingletonStatic : public boost::noncopyable
{
public:
    static T& getInstance()
    {
        static T single_instance;
        return single_instance;
    }
};

}
#endif