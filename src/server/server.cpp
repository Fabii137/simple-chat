#include <format>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>

#include "common/common.hpp"
#include "server/server.hpp"

Server::Server(const int domain, const int type, const int protocol,
               const unsigned int port)
    : c_Domain(domain), c_Type(type), c_Protocol(protocol), c_Port(port) {}

Server::~Server() {}

int Server::bind() {
  m_Sock = socket(c_Domain, c_Type, c_Protocol);
  if (m_Sock < 0) {
    perror("Creating socket failed");
    return -1;
  }

  const int opt = 1;
  if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    return -1;
  }

  sockaddr_in address{};
  address.sin_family = c_Domain;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(c_Port);

  m_SockAddr = *reinterpret_cast<sockaddr *>(&address);
  const int addrLen = sizeof(m_SockAddr);
  if (::bind(m_Sock, &m_SockAddr, addrLen) < 0) {
    perror("Bind failed");
    return -1;
  }

  return 0;
}

int Server::start() {
  if (listen(m_Sock, 3) < 0) {
    perror("Listen failed");
    return -1;
  }

  std::cout << "Server listening on port " << c_Port << "...\n";

  int addrLen = sizeof(m_SockAddr);
  while (true) {
    const int clientSock =
        accept(m_Sock, &m_SockAddr, reinterpret_cast<socklen_t *>(&addrLen));
    if (clientSock < 0) {
      perror("Accept failed");
      return -1;
    }

    std::thread(&Server::handleClient, this, clientSock).detach();
  }
}

void Server::handleClient(int clientSock) {
  char buffer[1024];

  // first message should be username
  const ssize_t bytes = read(clientSock, buffer, sizeof(buffer) - 1);
  if (bytes > 30) {
    return;
  }
  buffer[bytes] = '\0';
  const std::string username(buffer);
  const std::string decryptedUsername = Common::decrypt(username);
  {
    std::lock_guard lock(m_UsersMutex);
    m_UsersMap.insert(
        std::pair<int, const std::string>(clientSock, decryptedUsername));
  }

  broadcast(decryptedUsername + " connected");

  while (true) {
    const ssize_t bytesReceived = read(clientSock, buffer, sizeof(buffer) - 1);
    if (bytesReceived <= 0) {
      {
        std::lock_guard lock(m_UsersMutex);
        m_UsersMap.erase(clientSock);
      }
      close(clientSock);

      const std::string fullMessage = decryptedUsername + " disconnected";
      broadcast(fullMessage);
      break;
    }
    buffer[bytesReceived] = '\0';
    const std::string clientUsername = m_UsersMap[clientSock];

    const std::string message(buffer);
    const std::string decryptedMessage = Common::decrypt(message);

    const std::string fullMessage =
        std::format("{}: {}", clientUsername, decryptedMessage);
    broadcastExcept(clientSock, fullMessage);
  }
}

void Server::broadcast(const std::string &message) {
  std::cout << message << std::endl;
  std::lock_guard lock(m_UsersMutex);
  for (auto it = m_UsersMap.begin(); it != m_UsersMap.end(); ++it) {
    const int sock = it->first;
    const std::string encryptedMessage = Common::encrypt(message);
    send(sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);
  }
}

void Server::broadcastExcept(int clientSock, const std::string &message) {
  std::cout << message << std::endl;
  std::lock_guard lock(m_UsersMutex);
  for (auto it = m_UsersMap.begin(); it != m_UsersMap.end(); ++it) {
    const int sock = it->first;

    if (sock == clientSock) {
      continue;
    }

    const std::string encryptedMessage = Common::encrypt(message);
    send(sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);
  }
}
