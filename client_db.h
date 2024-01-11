#pragma once
// clidb

#include <list>

class Client;
class TcpServer;

class ClientDB {
 public:
  TcpServer* srv;

  ClientDB(TcpServer* srv);
  ~ClientDB();

  void init();

 private:
  std::list<Client*> client_;
};