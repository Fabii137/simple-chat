#include <print>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>


#include "server/server.hpp"




Server::Server(const int domain, const int type, const int protocol, const unsigned int port)
        : m_Domain(domain), m_Type(type), m_Protocol(protocol), m_Port(port) {
}

Server::~Server() {

}

int Server::bindServer() {
    m_Sock = socket(m_Domain, m_Type, m_Protocol);
    if (m_Sock < 0) {
        perror("Creating socket failed");
        return -1;
    }

    const int opt = 1;
    if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -1;
    }

    sockaddr_in address{};
    address.sin_family = m_Domain;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_Port);

    m_SockAddr = *reinterpret_cast<sockaddr *>(&address);
    const int addrLen = sizeof(m_SockAddr);
    if (bind(m_Sock, &m_SockAddr, addrLen) < 0) {
        perror("Bind failed");
        return -1;
    }

    return 0;
}

int Server::startListening() {
    if (listen(m_Sock, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "Server listening on port " << m_Port << "...\n";

    int addrLen = sizeof(m_SockAddr);
    while (true) {
        const int clientSock = accept(m_Sock, &m_SockAddr, reinterpret_cast<socklen_t *>(&addrLen));
        if (clientSock < 0) {
            perror("Accept failed");
            return -1;
        }

        std::thread(&Server::handleClient, this, clientSock).detach();
    }
}

void Server::handleClient(const int clientSock) {
    char buffer[1024];

    // first message should be username
    const ssize_t bytes = read(clientSock, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    const std::string username(buffer);
    {
        std::lock_guard lock(m_UsersMutex);
        m_UsersMap.insert(std::pair<int, const std::string>(clientSock, username));
    }
    broadcast(username + " connected");
    std::cout << username << " connected" << std::endl;


    while (true) {
        const ssize_t bytesReceived = read(clientSock, buffer, 1024);
        if (bytesReceived <= 0) {
            {
                std::lock_guard lock(m_UsersMutex);
                m_UsersMap.erase(clientSock);
            }
            close(clientSock);

            const std::string fullMessage = username + " disconnected";
            std::cout << fullMessage << std::endl;
            broadcast(fullMessage);
            break;
        }
        buffer[bytesReceived] = '\0';
        const std::string clientUsername = m_UsersMap[clientSock];

        const std::string fullMessage = std::format("{}: {}", clientUsername, buffer);
        broadcastExcept(clientSock, fullMessage);
        std::cout << fullMessage << std::endl;
    }
}

void Server::broadcast(const std::string& message) {
    std::lock_guard lock(m_UsersMutex);
    for (auto it = m_UsersMap.begin(); it != m_UsersMap.end(); ++it) {
        const int sock = it->first;

        send(sock, message.c_str(), message.size(), 0);
    }
}

void Server::broadcastExcept(int clientSock, const std::string& message) {
    std::lock_guard lock(m_UsersMutex);
    for (auto it = m_UsersMap.begin(); it != m_UsersMap.end(); ++it) {
        const int sock = it->first;

        if (sock == clientSock) {
            continue;
        }

        send(sock, message.c_str(), message.size(), 0);
    }
}

