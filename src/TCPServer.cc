#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "TCPServer.h"
#include "people.pb.h"

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

void TCPServer::start()
{
    int listenfd, connectfd, epollfd, sockfd;
    struct epoll_event ev, events[kMaxEpollEvents];
    ssize_t numbytes;
    char buf[kMaxDataSize];
    struct sockaddr_in client;
    socklen_t sin_size;
    epollfd  = epoll_create(1);
    if(epollfd < 0)
    {
        cout << "epoll_create error" << endl;
        return;
    }
    listenfd = bindAndListen();
    addEpoll(ev, epollfd, listenfd);
    while(1)
    {
        int fds = epoll_wait(epollfd, events, kMaxEpollEvents, -1);
        if(-1 == fds)
        {
            cout << "epoll_wait error";
            break;
        }
        for(int i = 0; i < fds; ++ i)
        {
            if(events[i].data.fd == listenfd)
            {
                connectfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&client), &sin_size);
                if(connectfd > 0)
                {
                    cout << "New Connetion from "
                        << "[" << inet_ntoa(client.sin_addr)
                        << ":" << ntohs(client.sin_port) << "]"
                        << " accept socket fd:" << connectfd
                        << endl;
                }
                else
                {
                    cout << "accept error, connectfd:" << connectfd;
                }
                setNonBlock(connectfd);
                if(addEpoll(ev, epollfd, connectfd) == -1)
                {
                    cout << "epoll_ctrl error";
                }
            }
            else if(events[i].events & EPOLLIN)
            {
                if((sockfd = events[i].data.fd) < 0)
                {
                    cout << "EPOLLIN error";
                    continue;
                }
                bzero(buf, kMaxDataSize);
                numbytes = recv(sockfd, buf, kMaxDataSize, 0);
                buf[numbytes] = '\0';
                string a = buf;
                cout << "You got a message form " << inet_ntoa(client.sin_addr) << endl;
                cout << "Client Message: " << a << endl;
                if(a == "GET PEOPLE")
                {
                    string data;
                    Beyond::People p;
                    p.set_name("beyond");
                    p.set_id(123);
                    p.set_email("459898432@qq.com");
                    p.SerializeToString(&data);
                    char bts[data.length()];
                    strcpy(bts, data.c_str());
                    send(sockfd, bts, sizeof(bts), 0);
                }
                else
                {
                    send(sockfd, "Client Error!\n", 16, 0);
                }
                close(sockfd);
            }
        }
    }
}
