#pragma once
#include <sys/socket.h>
#include <unordered_map>
#include <string>
#include <mutex>

class Server {
public:
    Server(int domain, int type, int protocol, unsigned int port);
    ~Server();

    int bindServer();
    int startListening();
private:
    void handleClient(int clientSock);
    void broadcast(const std::string& message);
    void broadcastExcept(int clientSock, const std::string& message);
private:
    const int m_Domain;
    const int m_Type;
    const int m_Protocol;
    const unsigned int m_Port;
    int m_Sock;
    sockaddr m_SockAddr;

    std::mutex m_UsersMutex;
    std::unordered_map<int, const std::string> m_UsersMap;
};