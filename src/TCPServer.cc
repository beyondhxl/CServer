#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include "TCPServer.h"
#include "TCPTask.h"
#include "people.pb.h"

using namespace std;

TCPServer::TCPServer()
    : _epollfd(-1)
    , _listenfd(-1)
{
}

TCPServer::~TCPServer()
{
}

void TCPServer::bindAndListen()
{
    struct sockaddr_in server;
    _listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = SO_REUSEADDR;
    setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(kPort);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(_listenfd, reinterpret_cast<struct sockaddr *>(&server), static_cast<socklen_t>(sizeof(struct sockaddr))))
    {
        cout << "bind error";
        return;

    }
    if(-1 == listen(_listenfd, kMaxListen))
    {
        cout << "listen error";
        return;
    }
}

void TCPServer::setNonBlock(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void TCPServer::start()
{
     _epollfd = epoll_create(1);
    if(_epollfd < 0)
    {
        cout << "epoll_create error" << endl;
        return;
    }
    bindAndListen();
    TCPTask *task = new TCPTask(_epollfd, _listenfd);
    task->setTaskCallback(this);
    task->enableFdRead();
    while(1)
    {
        std::vector<TCPTask *> tasks;
        int fds = epoll_wait(_epollfd, _events, kMaxEpollEvents, -1);
        if(-1 == fds)
        {
            cout << "epoll_wait error";
            break;
        }
        for(int i = 0; i < fds; ++ i)
        {
            TCPTask *task = static_cast<TCPTask *>(_events[i].data.ptr);
            task->setRevents(_events[i].events);
            tasks.push_back(task);
        }
        std::vector<TCPTask *>::iterator iter;
        for(iter = tasks.begin(); iter != tasks.end(); ++ iter)
        {
            (*iter)->handleReadEvent();
        }
    }
}

void TCPServer::onRead(int socketfd)
{
    if(socketfd == _listenfd)
    {
        int connectfd;
        struct sockaddr_in clientaddr;
        socklen_t clientlen;
        connectfd = accept(_listenfd, reinterpret_cast<struct sockaddr *>(&clientaddr), &clientlen);
        if(connectfd > 0)
        {
            cout << "New Connetion From "
                << "[" << inet_ntoa(clientaddr.sin_addr)
                << ":" << ntohs(clientaddr.sin_port) << "]"
                << " Accept Socketfd:" << connectfd
                << endl;
        }
        else
        {
            cout << "accept error, connectfd:" << connectfd;
        }
        setNonBlock(connectfd);
        TCPTask *task = new TCPTask(_epollfd, connectfd);
        task->setTaskCallback(this);
        task->enableFdRead();
    }
    else
    {
        if(socketfd < 0)
        {
            cout << "EPOLLIN error";
            return;
        }
        char buf[kMaxDataSize];
        bzero(buf, kMaxDataSize);
        int numbytes = recv(socketfd, buf, kMaxDataSize, 0);
        buf[numbytes] = '\0';
        string a = buf;
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
            send(socketfd, bts, sizeof(bts), 0);
        }
        else
        {
            send(socketfd, "Client Message Error!\n", 24, 0);
        }
        close(socketfd);
    }
}
