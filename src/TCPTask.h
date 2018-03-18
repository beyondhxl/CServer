#ifndef _TCPTASK_H_
#define _TCPTASK_H_

#include "TCPTaskCallback.h"

/*
 * TCP连接
 *
 */

class TCPTask
{
    public:
        // 构造函数
        explicit TCPTask(int epollfd, int socketfd);

        // 析构函数
        virtual ~TCPTask();

    public:
        // 设置连接回调函数
        inline void setTaskCallback(TCPTaskCallback *callback) { _callback = callback; }

        // 设置返回事件
        inline void setRevents(int revents) { _revents = revents; }

        // 加入到epoll文件描述符中
        void updateEpollEvent();

        // 使文件描述符可读
        void enableFdRead();

        // 处理可读事件
        void handleReadEvent();

    private:
        int                _epollfd;    // epoll文件描述符
        int                _socketfd;   // socket文件描述符
        int                _events;     // 注册的事件
        int                _revents;    // 返回的事件
        TCPTaskCallback   *_callback;   // 连接回调函数
};

#endif
