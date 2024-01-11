#pragma once
// clidb

class Client;
class TcpServer;

class ClientDB {
 public:
  TcpServer* tcp_srv;

  ClientDB(TcpServer* srv);

 private:
  std::list<Client*> client_;
}