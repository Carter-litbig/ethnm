#pragma once
// connmgr

#include <pthread.h>

class TcpServer;
// 2024-03-25 ispark: EthnmCore add
class EthnmCore;

class ConnectionManager {
 public:
  TcpServer* server;
<<<<<<< HEAD
<<<<<<< HEAD

  ConnectionManager(TcpServer* server);
  ~ConnectionManager();

  void StartThread();
  void StartThreadInternal();

 private:
  int fd; /* socket file descriptor */
  pthread_t* thread;
=======
  // 2024-03-25 ispark: nmcore add
  EthnmCore* nmcore;
  int master_sock_udp;
  int sender_sock_udp;

  ConnectionManager(TcpServer* server);

  // 2024-03-25 ispark: nmcore add
  ConnectionManager(EthnmCore* nmcore);

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
=======
  // 2024-03-25 ispark: nmcore add
  EthnmCore* nmcore;
  int master_sock_udp;
  int sender_sock_udp;

  ConnectionManager(TcpServer* server);

  // 2024-03-25 ispark: nmcore add
  ConnectionManager(EthnmCore* nmcore);

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
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9

  pthread_t* thread;
  pthread_mutex_t mutex;
  void error_break(const char* s);
<<<<<<< HEAD
>>>>>>> 7aae719 (add deleted file)
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
};