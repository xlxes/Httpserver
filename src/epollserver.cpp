#include "epollserver.h"

namespace httpserver
{

static const int MAX_EVENTS = 255;
static const int MAX_MMAP_SIZE = 10000;

/**
*  @brief 根据param初始化一个Epollserver对象
*/
Epollserver::Epollserver(Threadpool *pool, Parameters *param) : Tcpserver(pool, param->get_listen_port_()), param_(param)
{
    document_root_ = param_->get_doc_root_();
    default_file_ = param_->get_defaul_file_();
    file_mmap(); //将文件列表中的文件均加载到共享内存中
}

//一个事件fd,用于signt信号触发之后离开处理http的循环
int Epollserver::signal_fd_ = eventfd(0, 0);

/**
*  @brief 根据param的file_path_lists_获取文件的路径并依次打开，打开后的fd存放于file_fd_lists_数组
*  同时根据fd依次在共享空间中打开文件，最终将共享空间的首地址file_ptr与文件名称填入map<string,char*> http_file_中
*/
void Epollserver::file_mmap()
{
    vector<string> filelists = param_->get_file_path_lists_();
    file_fd_lists_.resize(filelists.size()); //重设文件列表大小
    for (size_t i = 0; i < filelists.size(); i++)
    {
        DEBUG("filelists[i]:%s\n", filelists[i].data());
        file_fd_lists_[i] = open(filelists[i].data(), O_RDONLY); //将文件依次只读打开后，fd存在list中
        if (file_fd_lists_[i] == -1)
        {
            WARN("open file %s error\n", filelists[i].data());
            continue;
        }
        //在共享内存中打开该文件并将其首地址赋值给file_ptr
        char *file_ptr = (char *)mmap(NULL, MAX_MMAP_SIZE, PROT_READ, MAP_SHARED, file_fd_lists_[i], 0);
        if (file_ptr == (char *)-1)
        {
            WARN("mmap error\n");
        }
        else
        {
            http_file_[filelists[i]] = file_ptr; //将文件名字与对应共享内存首地址结合起来存在map中
        }
    }
}

/**
*  @brief 析构函数：释放打开文件的fd与共享内存
*/
Epollserver::~Epollserver()
{
    for (size_t i = 0; i < file_fd_lists_.size(); i++) //依次关闭打开的文件fd
    {
        if (file_fd_lists_[i] >= 0)
            close(file_fd_lists_[i]);
    }
    for (auto it : http_file_) //依次关闭共享内存空间
    {
        munmap(it.second, MAX_MMAP_SIZE);
    }
}

/**
*  @brief 接收到ctrl+c信号，转到处理函数。向signal_fd_写入一个int数据，用于通知循环结束
*/
void Epollserver::sig_init_handle(int sig)
{
    uint64_t u = 1;                             //必须是uint64大小的，才可以读入
    auto rc = write(signal_fd_, &u, sizeof(u)); //向signal_fd_事件写入一个uint64
    if (rc != sizeof(u))
        WARN("sigint eventfd error!\n");
}

/**
*  @brief 添加事件fd到epoll_fd_中
*  @param int fd
*  @param int event_type (EPOLLIN)
*/
void Epollserver::add_event(int fd, int event_type)
{
    //创建一个当前fd的事件并添加到epollfd中
    epoll_event ev; //创建一个事件结构体
    ev.data.fd = fd;
    ev.events = event_type; //表示对应的文件描述符可以读
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0)
        WARN("add serverfd fail\n");
}

/**
*  @brief 删除事件fd到epoll_fd_中
*  @param int fd
*  @param int event_type (EPOLLIN)
*/
void Epollserver::del_event(int fd, int event_type)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = event_type;
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
}

/**
*  @brief 关闭客户端fd
*  @param int fd
*/
void Epollserver::close_client(int fd)
{
    if (client_fd_timer[fd] != nullptr) //防止多次删除
    {
        client_time_queue_.del_timer(client_fd_timer[fd]); //删除超时队列中的对应项
        delete client_fd_timer[fd];                        //删除该fd对应的Timer*类对象
    }
    client_fd_timer[fd] = nullptr;
    close(fd);
}

/**
*  @brief epoll处理主循环
*/
void Epollserver::epoll_loop()
{
    signal(SIGPIPE, SIG_IGN);        //忽略sigpipe信号
    signal(SIGINT, sig_init_handle); //挂接sigint信号
    if (socket_ptr->getfd() == -1)
        err_exit("create listen fd error");
    if (signal_fd_ < 0)
        err_exit("signal_fd error");
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ < 0)
        err_exit("create epoll fd error");
    //timerfd_create两个参数，第一个为系统实时时钟，第二个为非阻塞模式
    int time_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK); //创建一个定时器接口，通过读fd可以进行超时通知
    if (time_fd < 0)
        err_exit("create timefd error");
    struct itimerspec new_value;
    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) < 0)
        err_exit("get time error");
    new_value.it_value.tv_sec = 2;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_nsec = 0;
    if (timerfd_settime(time_fd, 0, &new_value, NULL) < 0)
        err_exit("set time error");

    add_event(socket_ptr->getfd(), EPOLLIN);
    add_event(signal_fd_, EPOLLIN);
    add_event(time_fd, EPOLLIN);
    int overtime = -1;
    bool run = true;
    epoll_event events[MAX_EVENTS];
    while (run)
    {
        int ret = epoll_wait(epoll_fd_, events, MAX_EVENTS, overtime);
        if (ret < 0 && errno != EINTR) //出错且不是中断造成
        {
            WARN("epoll wait error\n");
            break;
        }
        for (int i = 0; i < ret; i++)
        {
            if (events[i].data.fd == socket_ptr->getfd()) //如果是监听文件描述符
            {
                int client_fd = socket_ptr->accept();
                if (client_fd == -1) //连接失败
                    continue;
                if (client_fd > MAX_FD)
                    err_exit("client fd over limit");
                set_noblock(client_fd);
                add_event(client_fd, EPOLLIN);
                timer::Timer *new_timer = new timer::Timer(client_fd,
                                                           std::bind(&Epollserver::close_overtime_client, this, std::placeholders::_1),
                                                           time(NULL) + CLIENT_LIFE_TIME);
                //time(NULL)获得机器的时间
                client_fd_timer[client_fd] = new_timer;
                client_time_queue_.add_timer(new_timer);
                DEBUG("accept a new client[%d]\n", client_fd);
            }
            else if (events[i].data.fd == time_fd) //超时事件
            {
                uint64_t exp;
                ssize_t s = read(time_fd, &exp, sizeof(uint64_t));
                if (s != sizeof(uint64_t))
                    err_exit("time fd read error");
                else
                {
                    time_t cur_time = time(NULL);
                    while (!client_time_queue_.empty())
                    {
                        auto first_timer = client_time_queue_.top();
                        if (first_timer->get_overtime() < cur_time)
                            first_timer->overtime_callback(first_timer); //超时则执行超时函数
                        else
                            break;
                    }
                    DEBUG("client queue size:%d\n", client_time_queue_.size());
                }
                DEBUG("over time\n");
            }
            else if (events[i].data.fd == signal_fd_) //离开循环信号事件
            {
                INFO("Get a sigint signal.Exiting...\n");
                run = false;
                break;
            }
            else if (events[i].events & EPOLLIN) //客户端发来请求事件
            {
                DEBUG("receive a request from client[%d]\n", events[i].data.fd);
                //将处理函数、参数使用bind函数添加到线程池任务
                bool res = add_task_to_pool(std::bind(&Epollserver::client_event, this, static_cast<int>(events[i].data.fd)));
                if (res == false)
                    WARN("add task to pool fail\n");
                //client_event(events[i].data.fd); //无线程池处理
                del_event(events[i].data.fd, EPOLLIN);
            }
            else if (events[i].events & EPOLLRDHUP) //当客户端调用close时，进入该条件
            {
                DEBUG("EPOLLRDHUP\n");
                close_client(events[i].data.fd);
                del_event(events[i].data.fd, EPOLLIN);
            }
            else
            {
                WARN("unexpected thing happened\n");
            }
        }
    }
    socket_ptr->close();
}

/**
*  @brief 从客户端fd读取数据并解析，最后返回数据给fd
*  @param int fd
*/
void Epollserver::client_event(int fd)
{
    DEBUG("deal with client request client fd:%d\n", fd);
    char recv_buff[BUFSIZ];
    http_request request;
    int count = recv(fd, recv_buff, BUFSIZ, 0);
    if (count == 0) //连接断开
    {
        close_client(fd);
        DEBUG("client close\n");
        return;
    }
    else if (count < 0)
    {
        close_client(fd);
        WARN("client read error\n");
        return;
    }
    analy_httprequest(recv_buff, &request);
    if (request.method != "GET" && request.method != "POST")
    {
        httperror_501(fd); //方式不匹配
        close_client(fd);
        return;
    }
    else if (request.method == "GET")
    {
        response_httprequest(fd, &request);
    }
    else if (request.method == "POST")
    {
        close_client(fd);
    }
}

/**
*  @brief 解析http协议，结果放在http_request的结构体中
*  @param char *http_data
*  @param http_request *request
*/
void Epollserver::analy_httprequest(char *http_data, http_request *request)
{
    string httpdata = http_data; //新建string类型保存原数据
    vector<string> lines;        //将string按每一行分割
    stringsplit::split(httpdata, lines, "\r\n");
    if (lines.size() < 1 || lines[0].empty())
        return;
    //解析第一行请求行
    vector<string> chunk;
    stringsplit::split(lines[0], chunk, " ");
    if (chunk.size() < 3) //至少有三个参数
        return;
    // std::cout<<"运行了此处"<<request->method<<std::endl;
    request->method = chunk[0];
    request->url = chunk[1];
    request->version = chunk[2];

    //解析第二行后的key和value,不能用map存储，防止排序
    size_t i = 1;
    vector<string> header;
    for (; lines[i] != "\r\n"; ++i)
    {
        stringsplit::split(lines[i], header, ":"); //按照:分割
        if (header.size() == 2)
        {
            request->headers.insert(pair<string, string>(header[0], header[1]));
        }
        header.clear(); //清空
    }

    //解析body,此时lines[i]="\r\n"
    for (i = i + 1; i < lines.size(); i++)
    {
        request->body += lines[i];
        if (i != lines.size() - 1)
            request->body += "\r\n"; //补上每行的换行符
    }
}

/**
*  @brief 解析http的GET对象合法性，确定是否是file_server 还是 cgi_server
*  @param int fd
*  @param http_request *request
*/
void Epollserver::response_httprequest(int fd, http_request *request)
{
    //http的头部如右侧 GET /index.html HTTP/1.1
    //或者无文件名，则为GET / HTTP/1.1
    // for (auto it : request->headers)
    //     cout << it.first << ":" << it.second << endl;
    // cout << request->method << endl;
    // cout << request->url << endl;
    // cout << request->version << endl;
    // cout << request->body << endl;
    string path;
    if (request->url == "/")
        path = document_root_ + default_file_;
    else
        path = document_root_ + request->url; //有可能是目录或者文件
    DEBUG("path is %s\n", path.data());
    struct stat st;
    if (stat(path.data(), &st) == -1) //不是文件夹或者目录
    {
        WARN("can not find file\n");
        close_client(fd);
        return;
    }
    else
    {
        if (S_ISDIR(st.st_mode))
        {
            path = path + default_file_;
        }
        //CGI server
        if (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
        {
            cgi_server(fd, path);
            close_client(fd);
        }
        else //file_server
        {
            file_server(fd, path);
            close_client(fd);
        }
    }
}

void Epollserver::file_server(int fd, string &path)
{
    string send_buff;
    send_buff = "HTTP/1.0 200 OK\r\n"
                "connection:keep-alive\r\n"
                "Content-Type: text/html\r\n";
    // http_response response;
    // response.version = "HTTP/1.1";
    // response.code = "200";
    // response.desc = "OK";
    if (http_file_.find(path) == http_file_.end())
    {
        WARN("can not open the file:%s\n", path.data());
        httperror_404(fd);
    }
    else
    {
        int content_len = strlen(http_file_[path]);
        //response.headers.insert(pair<string, string>("Content-Length", to_string(content_len)));
        //将所有信息转为一个string类型
        send_buff += "Content-Length" + to_string(content_len) + "\r\n\r\n";
        // for (auto it : response.headers)
        // {
        //     send_buff += it.first + ":" + it.second + "\r\n";
        // }
        // send_buff += "connection:keep-alive\r\nContent-Type: text/html\r\n\r\n";
        send(fd, send_buff.data(), send_buff.size(), MSG_NOSIGNAL);         //发送head和字典信息
        send(fd, http_file_[path], strlen(http_file_[path]), MSG_NOSIGNAL); //发送文件
    }
}

void Epollserver::cgi_server(int fd, string &path)
{
    //待补充
}

/**
*  @brief 返回客户端文件无法找到
*  @param int fd
*/
void Epollserver::httperror_404(int fd)
{
    DEBUG("send the 404 infomation!\n");
    string msg = "HTTP/1.0 404 NOT FOUND\r\n"
                 "Content-Type: text/html\r\n\r\n"
                 "<HTML><TITLE>Not Found</TITLE>\r\n"
                 "<BODY><P>The server could not fulfill\r\n"
                 "your request because the resource specified\r\n"
                 "is unavailable or nonexistent.\r\n"
                 "</BODY></HTML>\r\n";
    send(fd, msg.data(), msg.size(), MSG_NOSIGNAL);
}

/**
*  @brief 返回客户端请求Method不理解
*  @param int fd
*/
void Epollserver::httperror_501(int fd)
{
    DEBUG("HTTP/1.0 501 Method Not Implemented\n");
    string msg = "HTTP/1.0 501 Method Not Implemented\r\n"
                 "Content-Type: text/html\r\n"
                 "\r\n"
                 "<HTML><HEAD><TITLE>Method Not Implemented\r\n"
                 "</TITLE></HEAD>\r\n"
                 "<BODY><P>HTTP request method not supported.\r\n"
                 "</BODY></HTML>\r\n";
    send(fd, msg.data(), msg.size(), MSG_NOSIGNAL);
}

} // namespace httpserver
