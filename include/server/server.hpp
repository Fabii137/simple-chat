#pragma once
#include <sys/socket.h>
#include "common/common.hpp"

class Server {
public:
    Server(int domain, int type, int protocol, unsigned int port);
    ~Server();

    int bindServer();
    int startListening();
    void closeServer() const;
private:
    const int m_Domain;
    const int m_Type;
    const int m_Protocol;
    const unsigned int m_Port;
    int m_Sock;
    sockaddr m_SockAddr;
    Common m_Common;
};