#pragma once
// clisvc

class Client;
class TcpServer;

class ClientService {
 public:
  TcpServer* tcp_srv;

  ClientService(TcpServer* srv);
  ~ClientService();

 private:
  
};