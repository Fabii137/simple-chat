#include "client/client.hpp"

int main() {
    Client client(AF_INET, SOCK_STREAM, 0, 8888);
    if (client.openConnection("127.0.0.1") < 0) {
        return -1;
    }
    client.start();
    return 0;
}
