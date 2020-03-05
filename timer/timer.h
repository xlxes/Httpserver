#ifndef TIMER_H_
#define TIMER_H_

#include <list>
#include <functional>

using std::function;
using std::list;

namespace timer
{

class Timer
{

private:
    //意思为一个传入Timer*的对象，无返回的函数 重命名为callback_func_
    typedef function<void(Timer *)> callback_func_; //定义声明回调函数的类型
    int fd_;
    time_t overtime_;
    callback_func_ overtime_callback_;  
    list<Timer *>::iterator iter_;  //表示该类实例在列表中的位置

public:
    Timer(int fd, callback_func_ func, time_t overtime = 0)
        : fd_(fd), overtime_(overtime), overtime_callback_(func) {}
    ~Timer() {}
    void set_callback_func(callback_func_ func)
    {
        overtime_callback_ = func;
    }
    void set_iter(list<Timer *>::iterator iter)
    {
        iter_ = iter;
    }
    void set_overtime(time_t overtime)
    {
        overtime_ = overtime;
    }
    void overtime_callback(Timer *timer)
    {
        if (overtime_callback_)
            overtime_callback_(timer);
    }
    time_t get_overtime() const
    {
        return overtime_;
    }
    int get_fd() const
    {
        return fd_;
    }
    list<Timer *>::iterator get_iter() const
    {
        return iter_;
    }
    bool operator<(const Timer &b)
    {
        if (overtime_ < b.overtime_)
            return true;
        else
            return false;
    }
};
} // namespace timer

#endif