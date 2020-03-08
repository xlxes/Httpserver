#include "logger.h"



//利用LoggerMutex类的全局变量来在堆上新建一个对象
Loggermutex* Loggermutex::logger_mutex = new Loggermutex();

void err_exit(string s)
{
    cout<<"error :"<<s<<endl;
    exit(0);
}