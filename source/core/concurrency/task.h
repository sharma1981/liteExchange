#ifndef _TASK_H_
#define _TASK_H_

#include <memory>
#include <functional>
#include <string>
#include <core/memory/heap_memory.h>

namespace core
{

using TaskReturnType = void*;

class Task : public core::Aligned<>
{
    public :
        // Constructor
        Task() = default;
        // Copy constructor
        Task(const Task& rhs) = default;
        // Assignment operator
        Task& operator= (const Task &rhs) = default;
        // Destructor
        ~Task() = default;

        template<typename Function, typename ...Args>
        Task(Function f, Args... args) : m_callback{std::bind(f, args...)}
        { }

        void execute()
        {
            // Execute callback & Transfer return value buffer to Task via void* return type
            m_returnValue = m_callback();
        }

        TaskReturnType getReturnValue()const
        {
            return m_returnValue;
        }

    private:
        std::function<void*()> m_callback;
        TaskReturnType m_returnValue;
};

using TaskPtr = std::unique_ptr<Task>;

}//namespace

#endif