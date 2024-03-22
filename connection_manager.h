#pragma once
// connmgr

#include <pthread.h>

#define MULTICAST_ADDR "239.0.127.255"
#define MULTICAST_PORT 13800

#define DUT_ID 126
#define DUT_MAC "02:00:00:00:20:00"

#define TESTER_ID 127
#define TESTER_MAC "02:00:00:00:40:00"

#define BUFSIZE 512
#define TTL_DEFAULT 1

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* server;

  ConnectionManager(TcpServer* server);
  ~ConnectionManager();

  void SetUdpSocket();

  void RecieveMulticast();
  void SendMulticast();

  void StartThread();
  void StartThreadInternal();

  void Stop();
  void StopThread();

 private:
  int fd; /* socket file descriptor */
  int master_sock_udp;
  pthread_t* thread;
  void error_break(char *s);
};