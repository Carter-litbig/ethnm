#pragma once
// clidb

#include <list>
#include <pthread.h>

class TcpClient;
class TcpServer;

class ClientDB {
 public:
  TcpServer* server;

  ClientDB(TcpServer* server);
  ~ClientDB();

  void Create();
  void Update(TcpClient* client);
  void Display();

  /* assignment_7_2 */
  void Copy(std::list<TcpClient*>* list);
  void Purge();

 private:
  std::list<TcpClient*> clients_;
  pthread_rwlock_t rwlock;
};