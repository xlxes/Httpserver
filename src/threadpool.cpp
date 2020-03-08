#include <signal.h>
#include "threadpool.h"

namespace httpserver
{

static struct timeval delay = {0, 2};
Threadpool::Threadpool(Parameters *param)
    : next_(0),
      threads_num_(0),
      started(false),
      boot_mutex_(),
      boot_pcond_(boot_mutex_),
      pool_run(false),
      pool_param_(param)
{
    threads_num_ = pool_param_->get_init_work_num_();
    threads_max_num = pool_param_->get_max_work_num_();
}

void Threadpool::start()
{
    if (started)      //已经开始了
        err_exit("start error");
    if (threads_num_ <= 0)  //初始线程数小于等于0
        err_exit("threads num init error");
    sem_init(&task_num_, 0, 0); //初始化信号量
    //建立n+1个线程，且均阻塞等待wait信号
    pthread_barrier_init(&pool_barrier_, NULL, threads_num_ + 1);
    //循环创建n个工作线程
    for (int i = 0; i < threads_num_; ++i)
    {
        //工作线程的入口函数均为thread_routine
        //thread_routine的传入参数则为该工作线程在数组中的下标
        Workthread *t = new Workthread(std::bind(&Threadpool::thread_routine, this, i));
        work_threads_.push_back(t);
        //t->state_ = BOOTING;
        t->start();         //初始化创建一个线程
        //boot_pcond_.wait(); //该工作线程运行到notify处
        t->state_ = READY;  //使该工作线程继续工作
    }
    pool_run = true;
    //创建一个线程，用于分发任务，线程入口函数为distribute_task
    distribute_thread_ = shared_ptr<Httpthread>(new Httpthread(std::bind(&Threadpool::distribute_task, this)));
    distribute_thread_->start();
    //等上述n+1个线程均准备好，则所有线程开始工作
    pthread_barrier_wait(&pool_barrier_);
    INFO("thread pool is ready to work.\n");
}

Workthread *Threadpool::get_next_work_thread()
{
    Workthread *next_work_thread;
    next_work_thread = work_threads_[next_];
    ++next_;
    if (static_cast<size_t>(next_) >= work_threads_.size())
        next_ = 0;
    return next_work_thread;
}

void Threadpool::thread_routine(int index)
{
    pthread_detach(pthread_self()); //分离线程
    //lu 32位无符号整数
    INFO("Generated a work thread,id is%lu\n", pthread_self());
    if (index >= (int)work_threads_.size())
        err_exit("index error");
    Workthread *this_work_thread = work_threads_[index];
    /*while (this_work_thread->state_ == BOOTING) //条件变量
    {
        Httpmutexguard lg(boot_mutex_);
        boot_pcond_.notify();
    }*/
    pthread_barrier_wait(&pool_barrier_);
    while (pool_run)
    {
        if (this_work_thread->work_empty())
        {
            {
                Httpmutexguard lg(this_work_thread->get_mutex());
                this_work_thread->state_ = IDLE;
            }
            this_work_thread->get_pcond().wait();  
        }
        if (pool_run == false)
            break;
        while (this_work_thread->work_empty() == false)
        {
            DEBUG("get a task.id is:%lu\n", pthread_self());
            (this_work_thread->pop())->do_work(); //循环执行该work线程中workqueue中的任务
        }
    }
    INFO("work thread %d exits.\n", index + 1);
}

void Threadpool::distribute_task()
{
    pthread_detach(pthread_self()); //分离线程
    //lu 32位无符号整数
    INFO("disribute thread started,id is%lu\n", pthread_self());
    while (pool_run)
    {
        sem_wait(&task_num_);
        if (pool_run == false)
            break;
        Workthread *selected_thread = get_next_work_thread();
        if (pool_work_queue_.empty())
            err_exit("pool work queue error");
        Work::Workptr work_to_past = pool_work_queue_.pop();
        if(work_to_past==nullptr)
            err_exit("get Workptr error");
        selected_thread->add_work(work_to_past);
        {
            Httpmutexguard lg(selected_thread->get_mutex());
            if (selected_thread->state_ == IDLE)
            {
                selected_thread->state_ = BUSY;
                selected_thread->get_pcond().notify(); //通知该线程去执行work
            }
        }
    }
    INFO("Distribute task thread exits.\n");
}

bool Threadpool::add_task_to_pool(taskfunc newtask)
{
    if (pool_work_queue_.size() > threads_max_num)
    {
        WARN("Thread pool is busy.queue size is %d.\n", pool_work_queue_.size());
        return false;
    }
    //调用一个Work的静态成员函数，创建一个新的Work类同时返回shared_ptr<Work>
    shared_ptr<Work> newwork = Work::creat_work(newtask);
    pool_work_queue_.push(newwork); //将新工作加到线程池的工作链表中
    DEBUG("add task to pool ok,work num is:%d\n",pool_work_queue_.size());
    sem_post(&task_num_);   //释放资源
    return true;
}

void Threadpool::close_pool()
{
    pool_run = false;
    sem_post(&task_num_);
    while (pthread_kill(distribute_thread_->get_thread_id(), 0) == 0)
    {
        delay = {0, 2};
        select(0, NULL, NULL, NULL, &delay);
    }
    sem_destroy(&task_num_);
    for (unsigned i = 0; i < work_threads_.size(); ++i)
    {
        while (pthread_kill(work_threads_[i]->work_thread_id(), 0) == 0)
        {
            work_threads_[i]->get_pcond().notify();
            delay = {0, 2};
            select(0, NULL, NULL, NULL, &delay);
        }
    }
    INFO("thread pool is closed successfully.\n");
}
} // namespace httpserver