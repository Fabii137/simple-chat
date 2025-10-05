#include <print>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <thread>

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
        perror("Invalid IP address");
        return -1;
    }

    if (connect(m_Sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("Connect failed");
        return -1;
    }
    getUsername();
    send(m_Sock, m_Username.c_str(), m_Username.size(), 0);
    std::cout << "Connected" << std::endl;
    return 0;
}

void Client::start() {
    std::thread receiver(&Client::receiverLoop, this);
    senderLoop();

    if (receiver.joinable()) {
        receiver.join();
    }
}


void Client::senderLoop() const {
    std::string message;
    while (true) {
        getline(std::cin, message);
        if (message == "/exit") {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        if (message.size() > 1023) {
            std::cout << "Message too large" << std::endl;
            continue;
        }

        send(m_Sock, message.c_str(), message.size(), 0);
    }
    shutdown(m_Sock, SHUT_RDWR);
    close(m_Sock);
}

void Client::receiverLoop() const {
    char buffer[1024];
    while (true) {
        ssize_t bytesReceived = read(m_Sock, buffer, 1024);
        if (bytesReceived <= 0) {
            std::cout << "Connection closed" << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "[Server] " << buffer << std::endl;
    }
}

void Client::getUsername() {
    std::string username;
    do {
        std::cout << "Enter username: ";
        getline(std::cin, username);
        Common::trim(username);

        if (username.size() > 30) {
            std::cout << "Maximum size for username is 30 characters" << std::endl;
            username = "";
        }
    } while (username.empty());
    m_Username = username;
}



