#pragma once
// clidb

#include <list>

class TcpClient;
class TcpServer;

class ClientDB {
 public:
  TcpServer* srv;

  ClientDB(TcpServer* srv);
  ~ClientDB();

  void create();
  void update(TcpClient* client);

 private:
  std::list<TcpClient*> clients_;
};