#pragma once
// tcpcli
#include <stdint.h>

class TcpServer;
class MsgHandler;

const int kBufferSize = 1024;

class TcpClient {
  public:
  uint32_t ip;
  uint16_t port;
  int fd;

  TcpServer* server;
  MsgHandler* msg_handler;

  TcpClient(TcpServer* server, uint32_t ip, uint16_t port, int fd);

  void Display();
};