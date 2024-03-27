#pragma once
// clidb

#include <list>
<<<<<<< HEAD
=======
#include <pthread.h>
>>>>>>> 7aae719 (add deleted file)

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
<<<<<<< HEAD

 private:
  std::list<TcpClient*> clients_;
=======

  /* assignment_7_2 */
  void Copy(std::list<TcpClient*>* list);
  void Purge();

 private:
  std::list<TcpClient*> clients_;
  pthread_rwlock_t rwlock;
>>>>>>> 7aae719 (add deleted file)
};