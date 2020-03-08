#ifndef WORK_THREAD_H_
#define WORK_THREAD_H_

#include <boost/noncopyable.hpp>
#include <memory>
#include <iostream>

#include "httpthread.h"
#include "httpcondition.h"
#include "workqueue.h"

using namespace httpmutex;
using namespace httpthread;
using namespace httpcondition;

namespace httpserver
{
enum thread_state
{
    BOOTING, //启动
    READY,   //就绪
    IDLE,    //空闲
    BUSY,    //忙
    QUIT
};

/**
*  @brief 工作线程类，该类包含有一个工作队列Workqueue、一个线程函数、一个Httpthread类型的智能指针、以及线程ID、两种锁、
*  @brief 
*/
class Workthread : boost::noncopyable
{
public:
    typedef std::function<void(void *)> Threadfunc;
    typedef std::shared_ptr<Work> Workptr;
    thread_state state_;

private:
    Threadfunc thread_func_;
    Httpmutex mutex_;
    Httpcondition pcond_;
    std::shared_ptr<Httpthread> thread_ptr;
    pthread_t thread_id_;
    Workqueue<Workptr> work_queue_;

public:
    Workthread(Threadfunc func)
        : thread_func_(func),
          mutex_(),
          pcond_(mutex_),
          thread_ptr(new Httpthread(thread_func_, this))
    {
    }

    ~Workthread() {}

    void start()
    {
        thread_ptr->start();
        thread_id_ = thread_ptr->get_thread_id();
    }

    pthread_t work_thread_id()
    {
        return thread_id_;
    }

    Httpmutex &get_mutex()
    {
        return mutex_;
    }

    Httpcondition &get_pcond()
    {
        return pcond_;
    }

    inline void add_work(Work::Workptr new_work);
    inline Work::Workptr pop();
    inline bool work_empty();
};

inline void Workthread::add_work(Work::Workptr newwork)
{
    work_queue_.push(newwork);
}

inline Work::Workptr Workthread::pop()
{
    Work::Workptr temp = work_queue_.pop();
    return temp;
}

inline bool Workthread::work_empty()
{
    return work_queue_.empty();
}

} // namespace httpserver

#endif