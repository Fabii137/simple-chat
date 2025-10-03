#include <print>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "client/client.hpp"

#include <cstring>

#include "common/common.hpp"



Client::Client(const int domain, const int type, const int protocol, const unsigned int port)
        : m_Domain(domain), m_Type(type), m_Protocol(protocol), m_Port(port) {
    Common common;
    std::println("Client::Client()");

    struct sockaddr_in serverAddress;
    const char *hello = "Hello from client";

    int sock = socket(domain, type, protocol);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = domain;
    serverAddress.sin_port = htons(port);

    if (inet_pton(domain, "127.0.0.1", &serverAddress.sin_addr) < 0) {
        perror("invalid address/address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    send(sock, hello, strlen(hello), 0);
    std::println("Message sent");
    close(sock);
}



Client::~Client() {
    std::println("Client::~Client()");
}

