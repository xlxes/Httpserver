/*

#include "httpcondition.h"
#include "errno.h"

namespace httpcondition
{
bool Httpcondition::waitForseconds(double seconds)
{
    struct timespec curtime;
    clock_gettime(CLOCK_REALTIME,&curtime);
    const int64_t KNanaoSecondsPerSecong = 1000000000; //10^9次方 1秒=KN纳秒
    int64_t nanoseconds = static_cast<int64_t>(seconds*KNanaoSecondsPerSecong);
    //计算期望等待的时间
    curtime.tv_sec += static_cast<time_t>((curtime.tv_nsec+nanoseconds)/KNanaoSecondsPerSecong);    
    curtime.tv_nsec = static_cast<long>((curtime.tv_nsec+nanoseconds)%KNanaoSecondsPerSecong);
    Httpmutexguard lg(mutex_);
    //以下pthread_cond_timedwait解锁mutex_,并等待pcond_的信号，如果在curtime时间之前有信号则返回0，否则返回非0
    //而如果阻塞失败直接显示超时退出会返回ETIMEDOUT
    return ETIMEDOUT == pthread_cond_timedwait(&pcond_,mutex_.getmutex(),&curtime);
}
} // namespace Httpcondition

*/