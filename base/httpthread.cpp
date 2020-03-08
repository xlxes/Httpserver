#include "httpthread.h"

namespace httpthread
{
    
//静态成员初始化
boost::atomic_int Httpthread::thread_num(0);

void Httpthread::start()
{
    if (!started_)
    {
        pthread_create(&id_, NULL, thread_func<Httpthread>, this);
        started_ = true;
    }
    else
        return;
}

void Httpthread::join()
{
    if (started_ && !joined_)
    {
        pthread_join(id_, NULL);
    }
    return;
}

void Httpthread::detach()
{
    if (started_ && !detached_ && !joined_)
    {
        pthread_detach(id_);
    }
    return;
}

} // namespace httpthread