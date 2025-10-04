#include <netinet/in.h>

#include "client/client.hpp"

int main() {
    Client client(AF_INET, SOCK_STREAM, 0, 8888);
    client.openConnection("127.0.0.1");
    if (client.sendMessage("Hello World!") < 0) {
        perror("Sending message failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}
