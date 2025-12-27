#pragma once

#include <string>
#include <sys/socket.h>

class Client {
public:
  Client(int domain, int type, int protocol, unsigned int port);
  ~Client();

  int open(const std::string &server_ip);
  void start();

private:
  void senderLoop() const;
  void receiverLoop() const;
  void getUsername();

private:
  const int c_Domain;
  const int c_Type;
  const int c_Protocol;
  const unsigned int c_Port;
  int m_Sock;
  std::string m_Username;
};
