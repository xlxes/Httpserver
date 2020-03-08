#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <semaphore.h>
#include "workthread.h"
#include "parameters.h"

namespace httpserver
{

using std::function;
using std::shared_ptr;
using std::vector;

class Threadpool : boost::noncopyable
{
public:
    typedef function<void()> taskfunc;

private:
    vector<Workthread *> work_threads_;
    shared_ptr<Httpthread> distribute_thread_;
    int next_;
    int threads_num_;
    bool started;
    Httpmutex boot_mutex_;
    Httpcondition boot_pcond_;
    pthread_barrier_t pool_barrier_;
    Httpmutex pool_mutex_;
    bool pool_run;
    sem_t task_num_;
    Workqueue<Work::Workptr> pool_work_queue_;
    Parameters *pool_param_;
    int threads_max_num;

public:
    Threadpool(Parameters *param);
    void start();
    void thread_routine(int index);
    void distribute_task();
    Workthread *get_next_work_thread();
    bool add_task_to_pool(taskfunc newtask);
    void close_pool();
    ~Threadpool()
    {
        //依次删除所有的工作线程，存在work_threads_数组中，类型为Workthread*
        for (unsigned i = 0; i < work_threads_.size(); ++i)
        {
            delete work_threads_[i];
        }
    }
};
} // namespace httpserver

#endif