#pragma once
// clidb

#include <list>

class Client;
class TcpServer;

class ClientDB {
 public:
  TcpServer* tcp_srv;

  ClientDB(TcpServer* srv);
  ~ClientDB();

 private:
  std::list<Client*> client_;
};