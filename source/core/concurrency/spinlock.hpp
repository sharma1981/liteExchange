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
        SpinLock() : m_yielding{ true }, m_spinCount{ BaseLock::DEFAULT_SPIN_COUNT }
        {
            unlock();
        }

        void setYieldingAndSpinCount(bool value, std::size_t spinCount)
        {
            m_yielding = value;
            m_spinCount = spinCount;
        }

        void lock() override
        {
            while (true)
            {
                if (m_yielding)
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
                else
                {
                    if (tryLock() == true)
                    {
                        return;
                    }
                }
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
        bool m_yielding;
        std::size_t m_spinCount;
        std::atomic_flag m_flag;
        // Move ctor deletion
        SpinLock(SpinLock&& other) = delete;
        // Move assignment operator deletion
        SpinLock& operator=(SpinLock&& other) = delete;
};

} // namespace

#endif