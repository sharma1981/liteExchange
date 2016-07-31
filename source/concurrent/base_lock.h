#ifndef _BASE_LOCK_H_
#define _BASE_LOCK_H_

#include <boost/noncopyable.hpp>
#include <string>

namespace concurrent
{

class BaseLock : public boost::noncopyable
{
    public :
        explicit BaseLock(const std::string lockName = "") : m_lockName(lockName)
        {
        }

        const std::string getName() const{ return m_lockName;  }
        // We provide those two methods
        // so that the derived classes can work with
        // std::lock_guard or std::unique_lock template classes
        virtual void lock() = 0;
        virtual void unlock() = 0;
        // try_lock can be used with std::unique_lock`s std::try_to_lock
        virtual bool try_lock() = 0;
    private :
        std::string m_lockName;
};

} // namespace

#endif