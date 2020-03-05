#include "logger.h"



//利用LoggerMutex类的全局变量来在堆上新建一个对象
LoggerMutex* LoggerMutex::logger_mutex = new LoggerMutex();

void err_exit(string s)
{
    cout<<"error :"<<s<<endl;
    exit(0);
}