#include <cstdio>
#include <cstdlib>

#include "server/server.hpp"

int main() {
  Server server(AF_INET, SOCK_STREAM, 0, 8888);
  if (server.bind() < 0) {
    return EXIT_FAILURE;
  }

  if (server.start() < 0) {
    return EXIT_FAILURE;
  }
}
