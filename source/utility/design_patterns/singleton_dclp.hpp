#ifndef _SINGLETON_DCLP_H_
#define _SINGLETON_DCLP_H_

#include <boost/noncopyable.hpp>
#include <concurrent/lock.hpp>

namespace utility
{
template <class T>
class SingletonDCLP : public boost::noncopyable
{
    public:
        static T * getInstance()
        {
            // If object is not initialised, acquire lock
            if (m_instance == nullptr)
            {
                m_lock.lock();
                // If simultaneously two access check and passed the first "if" condition
                // then only one who acquire the lock first create the instance
                if (m_instance == nullptr)
                {
                    m_instance = new T;
                }
                m_lock.unlock();
            }

            return m_instance;
        }
    private:
        static concurrent::Lock m_lock;
        static T* m_instance;
};

template <class T>
T* SingletonDCLP<T>::m_instance = nullptr;

template <class T>
concurrent::Lock SingletonDCLP<T>::m_lock;

} //namespace

#endif