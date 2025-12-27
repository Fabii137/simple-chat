#pragma once
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <unordered_map>

class Server {
public:
  Server(int domain, int type, int protocol, unsigned int port);
  ~Server();

  int bind();
  int start();

private:
  void handleClient(int clientSock);
  void broadcast(const std::string &message);
  void broadcastExcept(int clientSock, const std::string &message);

private:
  const int c_Domain;
  const int c_Type;
  const int c_Protocol;
  const unsigned int c_Port;
  int m_Sock;
  sockaddr m_SockAddr;

  std::mutex m_UsersMutex;
  std::unordered_map<int, const std::string> m_UsersMap;
};
