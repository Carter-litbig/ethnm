#pragma once
// clisvc

#include <list>

class TcpClient;
class TcpServer;

class ClientService {
 public:
  TcpServer* srv;

  ClientService(TcpServer* srv);
  ~ClientService();

  void startThread();
  void startThreadInternal();
  void stopThread();
  void listen(TcpClient* client);

  int getMaxFd();
  void copyClientFd(fd_set*);
  void addClient(TcpClient* client);

 private:
  int max_fd;
  fd_set active_fd_set;
  fd_set backup_fd_set;
  pthread_t* thread;
  std::list<TcpClient*> clients_;
};