#pragma once
// tcpcli
#include <stdint.h>

class TcpServer;
class MsgDelimiter;

const int kBufferSize = 1024;

class TcpClient {
  public:
  uint32_t ip;
  uint16_t port;
  int fd;

  uint32_t srv_ip;
  uint16_t srv_port;

  TcpServer* server;
  MsgDelimiter* msg_delimiter;

  TcpClient(TcpServer* server, uint32_t ip, uint16_t port, int fd);

  void Display();
  
  /* assignment_7_2 */
  void Abort();
};