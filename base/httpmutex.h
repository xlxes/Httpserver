#ifndef HTTP_MUTEX_H_
#define HTTP_MUTEX_H_

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace httpmutex
{

class Httpmutex : boost::noncopyable
{
private:
    pthread_mutex_t mutex_;

public:
    Httpmutex() //默认构造时初始化锁
    {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~Httpmutex()
    {
        pthread_mutex_destroy(&mutex_);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t *getmutex()
    {
        return &mutex_;
    }
};

class Httpmutexguard : boost::noncopyable
{
private:
    Httpmutex &mutex_;

public:
    explicit Httpmutexguard(Httpmutex &mutex) : mutex_(mutex)
    {
        mutex_.lock(); //构造时上锁
    }
    ~Httpmutexguard()
    {
        mutex_.unlock(); //析构时解锁
    }
};
} // namespace httpmutex
#endif