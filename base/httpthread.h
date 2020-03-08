#ifndef HTTP_THREAD_H_
#define HTTP_THREAD_H_

#include <boost/function.hpp>
#include <boost/atomic.hpp>

namespace httpthread
{

/**
 *  @brief 该函数用于使用pthread_creat时创建一个该类型的对象(此处为Httpthread)，同时运行其func_函数，运行完毕thread_num减1
 *  传入参数并不为0，而是一个Httpthread对象指针，只是需要内部从void*转换
 *  @param void* arg 实际为 T* arg
**/
template <typename T>
void *thread_func(void *arg)
{
    T *thisclass = (T *)arg;
    thisclass->func_();
    T::thread_num--;
    return nullptr;
}


/**
 *  @brief 一个线程类，包含静态变量thread_num  重点在于其构造函数
 * 该线程执行函数为func_,是一个被function为无输入无输出的函数
**/
class Httpthread
{
private:
    pthread_t id_;
    bool started_;
    bool joined_;
    bool detached_;

public:
    Httpthread(const Httpthread &) = delete;
    Httpthread &operator=(const Httpthread &) = delete;
    static boost::atomic_int thread_num;

    typedef std::function<void()> Threadfunc_; //使用function声明线程函数
    Threadfunc_ func_;                         //声明线程类中的函数
    /*重点部分*/
    template <typename _func, typename... _Args>  //_Args表示一个参数包，可以有不定个数的参数
    //_func &&_threadfun, _Args &&... _args 为右值引用传入参数
    explicit Httpthread(_func &&_threadfun, _Args &&... _args) : started_(false), joined_(false), detached_(false)
    {
        //std::forward实现完美转发，传入参数为右值引用时，根据传入参数的类型进行推导，_threadfun是右值则forward之后也为右值
        func_ = std::bind(std::forward<_func>(_threadfun), std::forward<_Args>(_args)...);
        thread_num++;
    }

    ~Httpthread() {}
    void start();
    void join();
    void detach();
    pthread_t get_thread_id() const { return id_; }
};

} // namespace httpthread

#endif