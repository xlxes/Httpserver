#include "parameters.h"
#include "epollserver.h"


int main(int argc, char *argv[])
{
    DEBUG("a http server of xlxes\n");
    httpserver::parameters::Parameters param(argc, argv);
    param.displayConfig();
    httpserver::Epollserver server(&param);
    server.epoll_loop();
}
