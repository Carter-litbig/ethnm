<<<<<<< HEAD
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
=======
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
>>>>>>> 7aae719 (add deleted file)
};