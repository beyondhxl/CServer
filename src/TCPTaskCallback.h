#ifndef _TCPTASKCALLBACK_H_
#define _TCPTASKCALLBACK_H_

class TCPTaskCallback
{
    public:
        virtual void onRead(int socketfd) {}
};

#endif
