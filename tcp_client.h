<<<<<<< HEAD
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
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
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
<<<<<<< HEAD
>>>>>>> 7aae719 (add deleted file)
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
};