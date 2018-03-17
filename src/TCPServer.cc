#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

#include "TCPServer.h"

using namespace std;


TCPServer::TCPServer()
{
}

TCPServer::~TCPServer()
{
}

int TCPServer::bindAndListen()
{
    struct sockaddr_in server;
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(kPort);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(listenfd, reinterpret_cast<struct sockaddr *>(&server), static_cast<socklen_t>(sizeof(struct sockaddr))))
    {
        cout << "bind error";
        return 0;

    }
    if(-1 == listen(listenfd, kMaxListen))
    {
        cout << "listen error";
        return 0;

    }
    return listenfd;
}

int TCPServer::addEpoll(struct epoll_event &ev, int &epollfd, int fd)
{
    int ret;
    ev.data.fd = fd;
    ev.events  = EPOLLIN /* EPOLLLT */;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    return ret;
}

void TCPServer::setNonBlock(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}
