#include "server.h"

namespace httpserver
{

/**
*  @brief Socket显示构造函数
*  
*  @para listenport
*  @para mode(1为udp 其余为tcp)
*/
Socket::Socket(int listenport, int mode) : port_(listenport), mode_(mode) //mode为1表示UDP传输，其余均为TCP
{
    if (mode == 1)
        fd_ = socket(AF_INET, SOCK_DGRAM, 0); //为1表示UDP
    else
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
}

/**
*  @brief Socket设置fd为复用模式
*/
void Socket::set_reuse()
{
    //设置fd为复用模式
    int reuse = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        err_exit("set fd fail");
    }
}

/**
*  @brief 绑定fd与本地地址端口
*/
void Socket::bind()
{
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port_);
    //使用域操作符::使用全局函数bind
    if (::bind(fd_, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        err_exit("bind fail");
}

/**
*  @brief 设置fd的监听队列，默认100000
*/
void Socket::listen()
{
    if (::listen(fd_, 100000) < 0)
        err_exit("listen fail");
}

/**
*  @brief 等待fd连接，返回新建的connectfd
*/
int Socket::accept()
{
    return ::accept(fd_, NULL, 0);
}

/**
*  @brief 关闭fd
*/
void Socket::close()
{
    ::close(fd_);
}

/**
*  @brief 初始化tcpfd,设置复用，绑定地址端口，设置监听端口
*/
Tcpserver::Tcpserver(Threadpool *pool, int listenport) : thread_pool_(pool), socket_ptr(new Socket(listenport, 0))
{
    socket_ptr->set_reuse();
    socket_ptr->bind();
    socket_ptr->listen();
}

/**
*  @brief 设置fd为非阻塞模式，用于epoll下的ET模式
*/
void Tcpserver::set_noblock(int fd_)
{
    //先获得文件的状态标识位，随后修改非阻塞那一项，随后重新赋值
    int old_op = fcntl(fd_, F_GETFL);
    int new_op = old_op | O_NONBLOCK;
    fcntl(fd_, F_SETFL, new_op);
}

} // namespace httpserver
