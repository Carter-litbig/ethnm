#pragma once
// connmgr

#include <pthread.h>

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* server;

  ConnectionManager(TcpServer* server);
  ~ConnectionManager();

  void StartThread();
  void StartThreadInternal();

 private:
  int fd; /* socket file descriptor */
  pthread_t* thread;
};