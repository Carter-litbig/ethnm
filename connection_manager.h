#pragma once
// connmgr

#include <pthread.h>
#include "nm_util.h"

class TcpServer;
// 2024-03-25 ispark: Ethnm add
class Ethnm;

class ConnectionManager {
 public:
  TcpServer* server;
  // 2024-03-25 ispark: ethnm add
  Ethnm* ethnm;
  int reciever_sock_udp;
  int sender_sock_udp;

  ConnectionManager(TcpServer* server);

  // 2024-03-25 ispark: ethnm add
  ConnectionManager(Ethnm* ethnm);

  ~ConnectionManager();

  void SetUdpSocket();

  void RecieveMulticast();
  void SendMulticast();

  void StartThread();
  void StartThreadInternal();

  void Stop();
  void StopThread();

 private:
  int fd_; /* socket file descriptor */

  pthread_t* thread_;
  void Error_break(const char* s);
};