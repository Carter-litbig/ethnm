#pragma once
// tcpcli
#include <stdint.h>

class TcpServer;

class TcpClient {
  public:
  uint32_t ip;
  uint16_t port;
  int fd;

  TcpServer* srv;

  TcpClient(TcpServer* srv, uint32_t ip, uint16_t port, int fd);

  void display();
};