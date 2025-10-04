#include <print>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "client/client.hpp"

Client::Client(const int domain, const int type, const int protocol, const unsigned int port)
        : m_Domain(domain), m_Type(type), m_Protocol(protocol), m_Port(port) {

}

Client::~Client() {

}

int Client::openConnection(const std::string& server_ip) {
    struct sockaddr_in serverAddress;
    m_Sock = socket(m_Domain, m_Type, m_Protocol);
    if (m_Sock < 0) {
        return -1;
    }

    serverAddress.sin_family = m_Domain;
    serverAddress.sin_port = htons(m_Port);

    if (inet_pton(m_Domain, server_ip.c_str(), &serverAddress.sin_addr) < 0) {
        return -1;
    }

    if (connect(m_Sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        return -1;
    }
    return 0;
}

int Client::sendMessage(const std::string& message) const {
    if (send(m_Sock, &message, message.size(), 0) < 0) {
        return -1;
    }
    std::println("Message '{}' sent", message);
    return 0;
}

void Client::closeConnection() const {
    close(m_Sock);
}


