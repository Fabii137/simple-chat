#include <print>
#include <netinet/in.h>
#include <unistd.h>

#include "server/server.hpp"


Server::Server(const int domain, const int type, const int protocol, const unsigned int port)
        : m_Domain(domain), m_Type(type), m_Protocol(protocol), m_Port(port) {
}

Server::~Server() {

}

int Server::bindServer() {
    struct sockaddr_in address;
    const int opt = 1;

    m_Sock = socket(m_Domain, m_Type, m_Protocol);
    if (m_Sock < 0) {
        return -1;
    }

    if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -1;
    }

    address.sin_family = m_Domain;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_Port);

    m_SockAddr = *reinterpret_cast<sockaddr *>(&address);
    const int addrLen = sizeof(m_SockAddr);
    if (bind(m_Sock, &m_SockAddr, addrLen) < 0) {
        return -1;
    }

    return 0;
}

int Server::startListening() {
    if (listen(m_Sock, 3) < 0) {
        return -1;
    }

    char buffer[1024];

    int addrLen = sizeof(m_SockAddr);

    while (true) {
        const int newClient = accept(m_Sock, &m_SockAddr, reinterpret_cast<socklen_t *>(&addrLen));
        if (newClient < 0) {
            return -1;
        }

        read(newClient, buffer, 1024);
        std::println("Message from client: {}", buffer);
        usleep(100000);
    }

    return 0;
}

void Server::closeServer() const {
    close(m_Sock);
}

