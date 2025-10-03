#pragma once

class Client {
public:
    Client(int domain, int type, int protocol, unsigned int port);
    ~Client();
private:
    int m_Domain;
    int m_Type;
    int m_Protocol;
    unsigned int m_Port;
};