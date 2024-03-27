#pragma once
// clisvc

#include <list>

class TcpClient;
class TcpServer;

class ClientService {
 public:
  TcpServer* server;

  ClientService(TcpServer* server);
  ~ClientService();

  void StartThread();
  void StartThreadInternal();
  void StopThread();
  void Listen(TcpClient* client);

  int GetMaxFd();
  void CopyClientFd(fd_set*);
  void AddClient(TcpClient* client);
<<<<<<< HEAD
=======

  void Stop();
>>>>>>> 7aae719 (add deleted file)

 private:
  int max_fd;
  fd_set active_fd_set;
  fd_set backup_fd_set;
  pthread_t* thread;
  std::list<TcpClient*> clients_;
};