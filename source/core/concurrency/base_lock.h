#ifndef _BASE_LOCK_H_
#define _BASE_LOCK_H_

#include <cstddef>
#include <string>

namespace core
{

class BaseLock
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
        // tryLock can be used with std::unique_lock`s std::try_to_lock
        virtual bool tryLock() = 0;

        // Spin count of this lock (critical section on Windows and pthread_mutex on Linux )
        // only applies to Windows as there is not a direct equivalent in all Linux systems
        // In HP`s systems, there is : pthread_mutexattr_setspin_np : http://h20564.www2.hpe.com/hpsc/doc/public/display?docId=emr_na-c02267594&lang=en-us&cc=us
        // But this does not apply to all distributions
        static const std::size_t DEFAULT_SPIN_COUNT = 0x00000400;
    private :
        std::string m_lockName;
};

} // namespace

#endif