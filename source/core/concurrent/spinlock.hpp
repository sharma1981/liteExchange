#ifndef _SPIN_LOCK_
#define _SPIN_LOCK_

#include <atomic>
#include <thread>
#include <cstddef>
#include "base_lock.h"

namespace core
{
    class SpinLock : public BaseLock
    {
    public:
        SpinLock() : m_spinCount{ BaseLock::DEFAULT_SPIN_COUNT }
        {
            unlock();
        }

        void setSpinCount(std::size_t spinCount)
        {
            m_spinCount = spinCount;
        }

        void lock() override
        {
            while (true)
            {
                for (std::size_t i(0); i < m_spinCount; i++)
                {
                    if (tryLock() == true)
                    {
                        return;
                    }
                }
                std::this_thread::yield(); // Where context switch happens
            }
        }

        bool tryLock() override
        {
            if (m_flag.test_and_set(std::memory_order_acquire) == true)
            {
                return false;
            }

            return true;
        }

        void unlock() override
        {
            m_flag.clear(std::memory_order_release);
        }

    private:
        std::size_t m_spinCount;
        std::atomic_flag m_flag;
        // Move ctor deletion
        SpinLock(SpinLock&& other) = delete;
        // Move assignment operator deletion
        SpinLock& operator=(SpinLock&& other) = delete;
    };

} // namespace

#endif