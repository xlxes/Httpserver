#ifndef EPOLLSERVER_H_
#define EPOLLSERVER_H_

#include <algorithm>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <string.h>
#include <queue>
#include <unordered_map>
#include <errno.h>

//本地文件
#include "stringsplit.h"
#include "parameters.h"
#include "server.h"
#include "timequeue.h"

using std::pair;
using std::unordered_map;
using std::to_string;

namespace httpserver
{

const int MAX_FD = 10000;

//记录接收的http结构
struct http_request
{
    string method;
    string url;
    string version;
    unordered_map<string, string> headers;
    string body;
};

class Epollserver : public Tcpserver
{
private:
    int epoll_fd_;
    string document_root_;
    string default_file_;
    parameters::Parameters*  param_;
    unordered_map<string, char *> http_file_; //string为文件路径对应的为共享内存的地址
    vector<int> file_fd_lists_;               //http文件打开后的fd列表
    static int signal_fd_;
    timer::Timerqueue client_time_queue_; //客户端超时队列
    timer::Timer *client_fd_timer[MAX_FD];     //客户端的fd以及对应的timer实例
public:
    Epollserver(parameters::Parameters *param);
    virtual ~Epollserver();
    void epoll_loop();
    void del_event(int fd, int event_type);
    void add_event(int fd, int event_type);
    void client_event(int fd);
    void close_client(int fd);
    void close_overtime_client(timer::Timer* overtime_fd)
    {
        close_client(overtime_fd->get_fd());
    };
    void file_mmap();
    static void sig_init_handle(int sig);

    void file_server(int fd, string &path);
    void cgi_server(int fd, string &path);
    void analy_httprequest(char *http_data, http_request *request);
    void response_httprequest(int fd, http_request *request);
    void httperror_501(int clientfd);
    void httperror_404(int clientfd);

    static const int CLIENT_LIFE_TIME = 5;
};
} // namespace httpserver

#endif //EPOLLSERVER_H_