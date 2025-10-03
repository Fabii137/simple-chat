#include <print>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

#include "server/server.hpp"
#include "common/common.hpp"

Server::Server(const int domain, const int type, const int protocol, const unsigned int port) {
    std::println("Client::Client()");
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    char buffer[1024];
    const int opt = 1;

    int server_fd = socket(domain, type, protocol);
    if (server_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = domain;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), addrLen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    int new_client = accept(server_fd, reinterpret_cast<sockaddr*>(&address), reinterpret_cast<socklen_t *>(&addrLen));
    if (new_client < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    read(new_client, buffer, 1024);
    std::println("Message from client: {}", buffer);

    close(new_client);
    close(server_fd);
}

Server::~Server() {
    std::println("Client::~Client()");
}

