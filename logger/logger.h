#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include "string"
#include "pthread.h"

using std::cout;
using std::endl;
using std::string;

class Loggermutex
{
private:
    pthread_mutex_t _mutex_;
    Loggermutex() //默认构造时初始化锁
    {
        pthread_mutex_init(&_mutex_, NULL);
    }
    Loggermutex(const Loggermutex &) = delete; //默认拷贝、复制和析构
    Loggermutex &operator=(const Loggermutex &) = delete;
    ~Loggermutex();

public:
    static Loggermutex *logger_mutex; //定一个全局静态变量,来调用函数
    void lock()
    {
        pthread_mutex_lock(&_mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&_mutex_);
    }
};


//#define LOGGER_DEBUG
#ifdef LOGGER_DEBUG
#define DEBUG(arg...)                        \
    {                                        \
        Loggermutex::logger_mutex->lock();   \
        printf("\033[32m[DEBUG]");           \
        printf(arg);                         \
        printf("\033[0m");                   \
        Loggermutex::logger_mutex->unlock(); \
    }
#else
#define DEBUG(arg...)
#endif

#define LOGGER_WARN
#ifdef LOGGER_WARN
#define WARN(arg...)                         \
    {                                        \
        Loggermutex::logger_mutex->lock();   \
        printf("\033[31m[WARN]");            \
        printf(arg);                         \
        printf("\033[0m");                   \
        Loggermutex::logger_mutex->unlock(); \
    }
#else
#define WARN(arg...)
#endif

#define LOGGER_INFO
#ifdef LOGGER_INFO
#define INFO(arg...)                         \
    {                                        \
        Loggermutex::logger_mutex->lock();   \
        printf("[INFO]");                    \
        printf(arg);                         \
        Loggermutex::logger_mutex->unlock(); \
    }
#else
#define INFO(arg...)
#endif

void err_exit(string s); //错误退出

#endif //  LOGGER_H_