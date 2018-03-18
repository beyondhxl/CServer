#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

const int kMaxListen = 20;      // 最大监听数量
const int kMaxDataSize = 20;    // 最大发送字节数
const int kMaxEpollEvents = 100;
const int kPort = 8888;

struct epoll_event;

class TCPServer
{
    public:
        // 构造函数
        TCPServer();

        // 析构函数
        virtual ~TCPServer();

    public:
        // 启动
        void start();

    public:
        void setNonBlock(int fd);

        // 绑定并监听
        int bindAndListen();

        // 加入到Epoll中
        int addEpoll(struct epoll_event &ev, int &epollfd, int fd);
};

#endif
