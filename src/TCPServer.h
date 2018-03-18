#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <sys/epoll.h>

#include "TCPTaskCallback.h"

const int kMaxListen = 20;      // 最大监听数量
const int kMaxDataSize = 20;    // 最大发送字节数
const int kMaxEpollEvents = 100;
const int kPort = 8888;

class TCPServer : public TCPTaskCallback
{
    public:
        // 构造函数
        TCPServer();

        // 析构函数
        virtual ~TCPServer();

        // 处理套接字读
        virtual void onRead(int socketfd);

    public:
        // 启动
        void start();

    public:
        // 设置套接字非阻塞
        void setNonBlock(int fd);

        // 绑定并监听
        void bindAndListen();

    private:
        int                _epollfd;                    // epoll文件描述符
        int                _listenfd;                   // 监听套接字描述符
        struct epoll_event _events[kMaxEpollEvents];    // epoll事件
};

#endif
