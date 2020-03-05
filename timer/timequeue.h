#ifndef TIMER_QUEUE_H_
#define TIMER_QUEUE_H_

#include "timer.h"
#include "httpmutex.h"

namespace timer
{
class Timerqueue
{
private:
    list<Timer *> timer_queue_;
    httpmutex::Httpmutex mutex_;

public:
    Timerqueue(){}
    ~Timerqueue(){}

    /**
* @brief 将一个Timer类实例指针插入到时间队列中，根据超时时间从短到长排列，即top最短 
* @param Timer *new_timer
*/
    void add_timer(Timer *new_timer)
    {
        httpmutex::Httpmutexguard m(mutex_);
        if (timer_queue_.empty()) //首端插入
        {
            timer_queue_.push_back(new_timer);
            new_timer->set_iter(timer_queue_.begin());
            return;
        }
        for (auto it = timer_queue_.begin(); it != timer_queue_.end(); it++) //中间插入
        {
            if (new_timer->get_overtime() < (*it)->get_overtime()) //按照超时事件排列 超时时间长的放在后面 top最短
            {
                auto new_iter = timer_queue_.insert(it, new_timer);
                new_timer->set_iter(new_iter);
                return;
            }
        }
        auto new_iter = timer_queue_.insert(timer_queue_.end(), new_timer); //尾端插入
        new_timer->set_iter(new_iter);
    }

    void del_timer(Timer *timer)
    {
        httpmutex::Httpmutexguard m(mutex_);
        timer_queue_.erase(timer->get_iter());
    }

    Timer *top()
    {
        httpmutex::Httpmutexguard m(mutex_);
        if (timer_queue_.empty())
            return nullptr;
        return timer_queue_.front();
    }

    void pop()
    {
        httpmutex::Httpmutexguard m(mutex_);
        if (timer_queue_.empty())
            timer_queue_.pop_front();
    }

    bool empty()
    {
        httpmutex::Httpmutexguard m(mutex_);
        return timer_queue_.empty();
    }

    int size()
    {
        httpmutex::Httpmutexguard m(mutex_);
        return timer_queue_.size();
    }
};

} // namespace timer

#endif