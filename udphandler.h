#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
class UDPHandler
{
public:
    using FNLOG_SOCKET = int;
    static const int FNLOG_INVALID_SOCKET = -1;

public:
    UDPHandler()
    {
        chunk_1_[0] = '\0';
        handler_ = FNLOG_INVALID_SOCKET;
    }
    ~UDPHandler()
    {
        if (handler_ != FNLOG_INVALID_SOCKET)
        {
            close();
        }
    }

    bool is_open()
    {
        return handler_ != FNLOG_INVALID_SOCKET;
    }

    void open()
    {
        handler_ = socket(AF_INET, SOCK_DGRAM, 0);
    }

    void close()
    {
        if (handler_ != FNLOG_INVALID_SOCKET)
        {
#ifndef WIN32
            ::close(handler_);
#else
            closesocket(handler_);
#endif 
            handler_ = FNLOG_INVALID_SOCKET;
        }
    }

    void write(unsigned int ip, unsigned short port, const char* data, int len)
    {
        if (handler_ == FNLOG_INVALID_SOCKET)
        {
            return;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = ip;
        int ret = sendto(handler_, data, len, 0, (struct sockaddr*) &addr, sizeof(addr));
        (void)ret;
    }
 
public:
    char chunk_1_[128];
    FNLOG_SOCKET handler_;
};
