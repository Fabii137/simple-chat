#include <cstdio>
#include <cstdlib>

#include "server/server.hpp"

int main() {
    Server server(AF_INET, SOCK_STREAM, 0, 8888);
    if (server.bindServer() < 0) {
        perror("Could not bind server");
        exit(EXIT_FAILURE);
    }

    if (server.startListening() < 0) {
        perror("Could not start listening");
        exit(EXIT_FAILURE);
    }
}