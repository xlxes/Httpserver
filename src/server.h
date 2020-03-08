#ifndef SERVER_H_
#define SERVER_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <boost/noncopyable.hpp>
#include <fcntl.h>
#include <memory> //包含智能指针
//本地文件
#include "threadpool.h"
#define LOGGER_WARN
#define LOGGER_INFO
//#define LOGGER_DEBUG
#include "logger.h"

namespace httpserver
{
//声明Socket类，同时设置为不可复制拷贝
class Socket : boost::noncopyable
{
private:
    int fd_;
    int port_;
    int mode_;

public:
    explicit Socket(int listenport, int mode);
    Socket() = delete; //禁止默认的构造函数
    int getfd() const { return fd_; };
    void set_reuse();
    void listen();
    void bind();
    int accept();
    void close();
};

//建立一个tcp基类，方便epollserver继承
class Tcpserver
{
protected:
    Threadpool *thread_pool_;
    std::shared_ptr<Socket> socket_ptr;

public:
    Tcpserver() = delete;
    explicit Tcpserver(Threadpool *pool, int listenport);
    virtual ~Tcpserver() {}
    void set_noblock(int fd);
    inline bool add_task_to_pool(std::function<void()> newtask)
    {
        return thread_pool_->add_task_to_pool(newtask);
    }
};
} // namespace httpserver

#endif //SERVER_H_