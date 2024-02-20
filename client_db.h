#pragma once
// clidb

#include <list>

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

 private:
  std::list<TcpClient*> clients_;
};