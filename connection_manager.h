#pragma once
// connmgr

#include <pthread.h>

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* srv;

  ConnectionManager(TcpServer* srv);
  ~ConnectionManager();

  void startThread();

 private:
  int fd; /* socket file descriptor */
  pthread_t* thread;
};