#ifndef _SINGLETON_DCLP_H_
#define _SINGLETON_DCLP_H_

#include <boost/noncopyable.hpp>
#include <concurrent/lock.hpp>
#include <atomic>

/*
	Meyers/Alexandrescu paper about issues with DLCP without memory fences : http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf

	Implementation reference : http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
*/

namespace utility
{
template <class T>
class SingletonDCLP : public boost::noncopyable
{
    public:
        static T * getInstance()
        {
			T* tmp = m_instance.load(std::memory_order_relaxed);
			std::atomic_thread_fence(std::memory_order_acquire);	// MEMORY-BARRIER SYNC BEGINS
			if (tmp == nullptr) 
			{
				std::lock_guard<concurrent::Lock> lock(m_lock);
				tmp = m_instance.load(std::memory_order_relaxed);
				if (tmp == nullptr) 
				{
					tmp = new T;
					std::atomic_thread_fence(std::memory_order_release); // MEMORY-BARRIER SYNC ENDS
					m_instance.store(tmp, std::memory_order_relaxed);	
				}
			}
			return tmp;
        }
    private:
        static concurrent::Lock m_lock;
        static std::atomic<T*> m_instance;
};

template <class T>
std::atomic<T*> SingletonDCLP<T>::m_instance = nullptr;

template <class T>
concurrent::Lock SingletonDCLP<T>::m_lock;

} //namespace

#endif