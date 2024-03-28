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
<<<<<<< HEAD
=======

  void Stop();
>>>>>>> 7aae719 (add deleted file)
=======

  void Stop();
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9

 private:
  int max_fd;
  fd_set active_fd_set;
  fd_set backup_fd_set;
  pthread_t* thread;
  std::list<TcpClient*> clients_;
};