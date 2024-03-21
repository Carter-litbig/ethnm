#pragma once
// connmgr

#include <pthread.h>

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* server;

  ConnectionManager(TcpServer* server);
  ~ConnectionManager();

  void UdpManager();

  void StartThread();
  void StartThreadInternal();

  void Stop();
  void StopThread();

 private:
  int fd; /* socket file descriptor */
  pthread_t* thread;
  void error_break(char *s);
};