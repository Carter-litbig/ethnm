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

 private:
  std::list<TcpClient*> clients_;
};