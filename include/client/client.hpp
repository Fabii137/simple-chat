#pragma once

#include <sys/socket.h>
#include <string>

class Client {
public:
    Client(int domain, int type, int protocol, unsigned int port);
    ~Client();

    int openConnection(const std::string& server_ip);
    void start();
private:
    void senderLoop() const;
    void receiverLoop() const;
    void getUsername();
private:
    const int m_Domain;
    const int m_Type;
    const int m_Protocol;
    const unsigned int m_Port;
    int m_Sock;
    std::string m_Username;
};