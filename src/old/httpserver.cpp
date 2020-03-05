#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h> //包含控制文件描述符的函数
#include <string>
#include <iostream>
//本地头文件
#include "httpdeal.h"
#include "parameters.h"



//using namespace std;

const int max_epollevent = 1000; //最大监听的事件数
const int recv_maxlen = 1024;    //最长1024个字节
const int serverport = 8080;     //监听端口

//传入clientfd和epollfd,处理读取和发送的事件，若失败，删除clientfd
void epoll_read_event(int clientfd, int epollfd);

void setfd_nonblock(int fd) //设置文件描述符为非阻塞方式
{
    //先获得文件的状态标识位，随后修改非阻塞那一项，随后重新赋值
    int old_op = fcntl(fd, F_GETFL);
    int new_op = old_op | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_op);
}

void err_exit(string s) //错误退出函数
{
    cout << "error:" << s << endl;
    exit(0);
}

int creat_fd(const int port)
{
    struct sockaddr_in serveraddr;
    socklen_t serverlen = sizeof(serveraddr);
    memset(&serveraddr, 0, serverlen);
    //设置服务器地址
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //建立socketfd
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
        err_exit("creat fd fail");
    //设置fd为复用模式
    int reuse = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        err_exit("setfd fail");
    //bind绑定端口、地址
    if (bind(serverfd, (sockaddr *)&serveraddr, serverlen) < 0)
        err_exit("bind fail");
    //设置监听队列
    if (listen(serverfd, 20) < 0)
        err_exit("listen fail");
    return serverfd;
}

//将文件描述符放入epollfd中
bool addtoepollfd(int &epollfd, int &fd, int cmd)
{
    //创建一个当前fd的事件并添加到epollfd中
    struct epoll_event ev; //创建一个事件结构体
    ev.data.fd = fd;
    ev.events = cmd; //表示对应的文件描述符可以读
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0)
        err_exit("add serverfd fail");
    return true;
}

int main(int argc,char *argv[])
{

    httpserver::parameters::Parameters para(argc,argv);
    para.displayConfig();
    while(1);
    //创建服务器套接字
    int serverfd = creat_fd(serverport);
    setfd_nonblock(serverfd); //设置为非阻塞模式
    //创建一个epoll文件描述符
    int epollfd = epoll_create(1); //此处的1为动态设置
    if (epollfd < 0)
        err_exit("creat epoll fail");
    addtoepollfd(epollfd, serverfd, EPOLLIN);
    //创建一个列表用于存放wait所返回的events
    //所有被触发的事件都会存放在这个数组中
    struct epoll_event events[max_epollevent] = {0};
    cout << "init ok,waiting for events" << endl;
    while (1)
    {
        //等待事件
        int number = epoll_wait(epollfd, events, max_epollevent, -1);
        //结束等待,即有事件触发
        if (number > 0)
        {
            //遍历所有事件
            for (int i = 0; i < number; i++)
            {
                int eventfd = events[i].data.fd;
                //如果触发的事件是serverfd即说明有连接请求
                if (eventfd == serverfd)
                {
                    cout << "new client request" << endl;
                    struct sockaddr_in clientaddr;
                    socklen_t clientlen = sizeof(clientaddr);
                    int connfd = accept(serverfd, (struct sockaddr *)&clientaddr, &clientlen);
                    setfd_nonblock(connfd);
                    //将新的文件描述符放到监听列表中
                    addtoepollfd(epollfd, connfd, EPOLLIN);
                }
                else
                {
                    epoll_read_event(eventfd, epollfd);
                }
            }
        }
    }
}

void epoll_read_event(int clientfd, int epollfd)
{
    char recv_buff[recv_maxlen] = {0}; //缓存区

    int count = recv(clientfd, recv_buff, recv_maxlen, 0);
    if (count == 0)
    {
        cout << "client close" << endl;
        close(clientfd);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, NULL);
    }
    else if (count < 0)
        cout << "read error" << endl;
    if (count > 0)
    {
        cout << recv_buff << endl;
        // cout << "------------http请求解析------------" << endl;
        // struct http_request request;
        // deal_request(rcv_buff, &request);
        // cout << "method is:" << request.method << endl;
        // cout << "url is:" << request.url << endl;
        // cout << "version is:" << request.version << endl;
        // cout << "key&value" << endl;
        // for (auto it = request.headers.begin();it!=request.headers.end();++it)
        //     cout << it->first << ":" << it->second << endl;
        // cout << "body is:" << request.body << endl;
        string senddata;
        //返回固定的http响应
        doRespone(senddata);
        //MSG_NOSIGNAL禁止send()函数向系统发送常消息
        send(clientfd, senddata.c_str(), senddata.size(), MSG_NOSIGNAL);
        close(clientfd);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, NULL);
    }
}
