#include "parameters.h"
#include "epollserver.h"
#include "threadpool.h"

using namespace httpserver;

int main(int argc, char *argv[])
{
    DEBUG("a http server of xlxes\n");
    Parameters param(argc, argv);
    param.displayConfig();
    Threadpool pool(&param);
    pool.start();
    Epollserver server(&pool, &param);
    server.epoll_loop();
    pool.close_pool();
    return 0;
}
