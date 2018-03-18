#include <sys/epoll.h>
#include <stdlib.h>

#include "TCPTask.h"

TCPTask::TCPTask(int epollfd, int socketfd)
    : _epollfd(epollfd)
    , _socketfd(socketfd)
    , _events(0)
    , _revents(0)
    , _callback(NULL)
{
}

TCPTask::~TCPTask()
{

}

void TCPTask::enableFdRead()
{
    _events |= EPOLLIN;
    updateEpollEvent();
}

void TCPTask::updateEpollEvent()
{
    struct epoll_event ev;
    ev.data.ptr = this;
    ev.events   = _events;
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, _socketfd, &ev);
}

void TCPTask::handleReadEvent()
{
    if(_revents & EPOLLIN)
    {
        _callback->onRead(_socketfd);
    }
}
