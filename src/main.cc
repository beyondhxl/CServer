#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <iostream>

#include "TCPServer.h"
#include "people.pb.h"

using namespace std;

int main(int args, char* argv[])
{
    int listenfd, connectfd, epollfd, sockfd;
    struct epoll_event ev, events[kMaxEpollEvents];
    ssize_t numbytes;
    char buf[kMaxDataSize];
    struct sockaddr_in client;
    socklen_t sin_size;
    TCPServer server;
    epollfd  = epoll_create(1);
    if(epollfd < 0)
    {
        cout << "epoll_create error" << endl;
        return -1;
    }
    listenfd = server.bindAndListen();
    server.addEpoll(ev, epollfd, listenfd);
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
                    continue;
                }
                server.setNonBlock(connectfd);
                if(server.addEpoll(ev, epollfd, connectfd) == -1)
                {
                    cout << "epoll_ctrl error";
                    continue;
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
    return 0;
}
