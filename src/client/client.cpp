#include <arpa/inet.h>
#include <format>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <ostream>
#include <thread>

#include "client/client.hpp"
#include "common/common.hpp"

Client::Client(int domain, int type, int protocol, unsigned int port)
    : c_Domain(domain), c_Type(type), c_Protocol(protocol), c_Port(port) {}

Client::~Client() {}

int Client::open(const std::string &server_ip) {
  struct sockaddr_in serverAddress;
  m_Sock = socket(c_Domain, c_Type, c_Protocol);
  if (m_Sock < 0) {
    perror("Creating socket failed");
    return -1;
  }

  serverAddress.sin_family = c_Domain;
  serverAddress.sin_port = htons(c_Port);

  if (inet_pton(c_Domain, server_ip.c_str(), &serverAddress.sin_addr) < 0) {
    perror("Invalid IP address");
    return -1;
  }

  if (connect(m_Sock, reinterpret_cast<struct sockaddr *>(&serverAddress),
              sizeof(serverAddress)) < 0) {
    perror("Connect failed");
    return -1;
  }
  getUsername();
  std::string encryptedMessage = Common::encrypt(m_Username);
  send(m_Sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);
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
    std::string fullMessage = std::format("{}: {}", m_Username, message);
    std::cout << fullMessage << std::endl;

    std::string encryptedMessage = Common::encrypt(message);
    send(m_Sock, encryptedMessage.c_str(), encryptedMessage.size(), 0);
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

    std::string receivedMessage(buffer);
    std::string decryptedMessage = Common::decrypt(receivedMessage);
    std::cout << decryptedMessage << std::endl;
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
      username.clear();
    }
  } while (username.empty());
  m_Username = username;
}
