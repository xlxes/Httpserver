#ifndef HTTP_CONDITION_H_
#define HTTP_CONDITION_H_

#include "httpmutex.h"

using namespace httpmutex;

namespace httpcondition
{
class Httpcondition : boost::noncopyable
{
    private:
    Httpmutex& mutex_;
    pthread_cond_t pcond_;  //条件变量

    public:
    explicit Httpcondition(Httpmutex& mutex):mutex_(mutex)
    {
        pthread_cond_init(&pcond_,NULL);   //初始化一个条件变量
    }

    ~Httpcondition()
    {
        pthread_cond_destroy(&pcond_);  //销毁条件变量
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyall()
    {
        pthread_cond_broadcast(&pcond_);
    }
    
    void wait()
    {
        Httpmutexguard lg(mutex_);
        pthread_cond_wait(&pcond_,mutex_.getmutex());   //互斥量mutex_被解锁，同时等待pcond_的信号
    }

    //bool waitForseconds(double seconds);

};
} // namespace httpcondition

#endif