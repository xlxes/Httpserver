#ifndef WORK_QUEUE_H_
#define WORK_QUEUE_H_

#include "httpmutex.h"
#include "boost/atomic.hpp"
#include "logger.h"

using namespace httpmutex;

namespace httpserver 
{
/**
*  @brief Work类，包含一个用function声明的函数和一个用于指向Work类的智能指针
*  @brief 成员函数：dowork()执行function函数
*  @brief 成员函数：creat_work（）根据work_func创建一个该类，并返回指向该类的sharedptr
*  @param 传入一个function<void()>函数
*/
class Work
{
public:
    typedef std::function<void()> work_func;
    typedef std::shared_ptr<Work> Workptr;

private:
    work_func work_;

public:
    explicit Work(work_func work) : work_(work)
    {
    }
    ~Work() {}

    static Workptr creat_work(work_func work)
    {
        return Workptr(new Work(work));
    }

    inline void do_work();
};

inline void Work::do_work()
{
    if (work_ == nullptr)
    {
        WARN("No work to do\n");
    }
    else
    {
        work_();
    }
}

/**
*  @brief 模板类节点，包含一个类型为T的对象和一个指向下一节点的指针
*/
template <class T>
struct node
{
    node *next;
    T work;
};


/**
*  @brief 模板类，依据以上节点实现一个链表，head_指向头（不含信息），tail指向最后一个节点
*  @brief 实现pop push empty top size等基本操作
*/
template <class T>
class Workqueue
{
private:
    node<T> *head_;
    node<T> *tail_;
    Httpmutex mutex_;
    boost::atomic_int queue_size_;

public:
    Workqueue() : queue_size_(0)
    {
        head_ = new node<T>;
        head_->next = nullptr;
        tail_ = head_;
    }

    void push(T work)
    {
        Httpmutexguard lg(mutex_);
        node<T> *newnode = new node<T>;
        newnode->next = nullptr;
        newnode->work = work;
        tail_->next = newnode;
        tail_ = newnode;
        queue_size_++;
    }

    bool empty()
    {
        Httpmutexguard lg(mutex_);
        if (tail_ == head_)
            return true;
        return false;
    }

    T pop()
    {
        Httpmutexguard lg(mutex_);
        if (tail_ == head_)
            return nullptr;
        node<T> *temp = head_->next;
        if (tail_ == temp)
            tail_ = head_;
        head_->next = temp->next;
        T work_to_pop = temp->work;
        delete temp;
        queue_size_--;
        return work_to_pop;
    }

    T top()
    {
        Httpmutexguard lg(mutex_);
        return head_->next->work;
    }

    int size()
    {
        return (int)queue_size_;
    }

    ~Workqueue()
    {
        while (head_->next != nullptr)
        {
            node<T> *delnode = head_->next;
            head_->next = delnode->next;
            delete delnode;
        }
        delete head_;
    }
};

} // namespace work

#endif