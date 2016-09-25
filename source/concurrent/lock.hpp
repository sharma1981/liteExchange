#ifndef _LOCK_H_
#define _LOCK_H_

/*
    Dealing with issues and fault finding :
 *
 * 1. If you set a thread to be robust , if the holder thread dies , our mutex will be unlocked
 * 2. If you want recursive access, set the type to RECURSIVE (On Windows it is always recursive )
 * 3. If you use an ERROR_CHECKING mutex and  trying to lock the same mutex twice or trying to unlock
 * a mutex that you didn`t lock ,will return EDEADLK
 * 4. Priority Inversion considerations : If you set the priority to be "inheritance"
 * the lowest priority thread which blocks a higher priority thread , will be temporarily be modified
 * and it will have a more important priority than the waiting high priority thread which will prevent
 * medium priority threads to make the high priority thread starve
 */

#include <cassert>

#ifdef __linux__
#include <pthread.h>
#elif _WIN32
#include <windows.h>
#endif

#include <string>

#define DEFAULT_SPIN_COUNT 0x00000400
// Spin count of this lock (critical section on Windows and pthread_mutex on Linux )
// only applies to Windows as there is not a direct equivalent in all Linux systems
// In HP`s systems, there is : pthread_mutexattr_setspin_np : http://h20564.www2.hpe.com/hpsc/doc/public/display?docId=emr_na-c02267594&lang=en-us&cc=us
// But this does not apply to all distributions

#include <concurrent/base_lock.h>
#include <concurrent/profiling/lock_profiler.h>

namespace concurrent
{

// For lock types, see http://linux.die.net/man/3/pthread_mutex_lock
enum class LockType { DEFAULT, NORMAL, ERROR_CHECK, RECURSIVE };

class Lock : public BaseLock
{
    public :

        Lock(const std::string lockName, unsigned long spinCount)
        : BaseLock(lockName), m_spinCount{ spinCount }, m_lockType(LockType::DEFAULT), m_robustness{false}
        {
            assert(m_spinCount > 0);
#ifdef __linux__
            resetAttribute();
#elif _WIN32
            InitializeCriticalSectionAndSpinCount(&m_criticalSection, m_spinCount);
#endif
		}

		Lock() : Lock("", DEFAULT_SPIN_COUNT) // CPP11 Constructor delegation
		{
		}

        explicit Lock(const std::string lockName) : Lock(lockName, DEFAULT_SPIN_COUNT) // CPP11 Constructor delegation
        {
        }

        ~Lock()
        {
#ifdef __linux__
            pthread_mutex_destroy(&m_mutex);
            pthread_mutexattr_destroy(&m_mutexAttribute);
#elif _WIN32
            DeleteCriticalSection(&m_criticalSection);
#endif
        }

        void lock() override
        {
            LOCK_PROFILER_START;
#ifdef __linux__
            pthread_mutex_lock(&m_mutex);
#elif _WIN32
            EnterCriticalSection(&m_criticalSection);
#endif
        }

        bool try_lock() override
        {
            bool ret{false};
#ifdef __linux__
            if(pthread_mutex_trylock(&m_mutex) == 0 )
            {
                ret = true;
            }
#elif _WIN32
            if (TryEnterCriticalSection(&m_criticalSection) != 0 )
            {
                ret = true;
            }
#endif
            if(ret == true)
            {
                LOCK_PROFILER_START;
            }
            return ret;
        }

        void unlock() override
        {
#ifdef __linux__
            pthread_mutex_unlock(&m_mutex);
#elif _WIN32
            LeaveCriticalSection(&m_criticalSection);
#endif
            LOCK_PROFILER_END;
            LOCK_PROFILER_TRACE;
        }

        unsigned long getSpinCount() const
        {
            return m_spinCount;
        }

        void resetAttribute()
        {
#ifdef __linux__
            pthread_mutexattr_init (&m_mutexAttribute);
            applyAttributeToMutex();
#endif
        }
        void setType(LockType type)
        {
#ifdef __linux__
            m_lockType = type;
            int nativeType = Lock::getNativeType(type);
            pthread_mutexattr_settype(&m_mutexAttribute, nativeType);
            applyAttributeToMutex();
#endif
        }

        void setRobustness(bool value)
        {
#ifdef __linux__
            m_robustness = value;
            int robustness = PTHREAD_MUTEX_STALLED;
            if(value == true)
            {
                m_robustness = PTHREAD_MUTEX_ROBUST;
            }
            pthread_mutexattr_setrobust(&m_mutexAttribute, robustness);
            applyAttributeToMutex();
#endif
        }

        void setPriorityInversionGuard()
        {
#ifdef __linux__
            pthread_mutexattr_setprotocol (&m_mutexAttribute, PTHREAD_PRIO_INHERIT);
            applyAttributeToMutex();
#endif
        }

        static int getNativeType(LockType type)
        {
            int ret{0};
#ifdef __linux__
            switch(type)
            {
                case LockType::DEFAULT:
                    ret = PTHREAD_MUTEX_DEFAULT;
                    break;
                case LockType::NORMAL:
                    ret = PTHREAD_MUTEX_NORMAL;
                    break;
                case LockType::ERROR_CHECK:
                    ret = PTHREAD_MUTEX_ERRORCHECK;
                    break;
                case LockType::RECURSIVE:
                    ret = PTHREAD_MUTEX_RECURSIVE;
                    break;
                default:
                    break;
            }
#endif
            return ret;
        }

    private :
#ifdef __linux__
        pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutexattr_t m_mutexAttribute;

        void applyAttributeToMutex()
        {
            pthread_mutex_init (&m_mutex, &m_mutexAttribute);
        }
#elif _WIN32
        CRITICAL_SECTION m_criticalSection;
#endif
        unsigned long m_spinCount;      // Only Windows
        LockType m_lockType;            // Applies to Linux, always on Windows
        bool m_robustness;              // Applies to Linux , if a thread dies the robust mutex it was holding will be unlocked

        DECLARE_LOCK_PROFILER;

        // Move ctor deletion
        Lock(Lock&& other) = delete;
        // Move assignment operator deletion
        Lock& operator=(Lock&& other) = delete;

};

} // namespace

#endif