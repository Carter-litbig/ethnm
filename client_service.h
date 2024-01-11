#pragma once
// clisvc

class Client;
class TcpServer;

class ClientService {
 public:
  TcpServer* srv;

  ClientService(TcpServer* srv);
  ~ClientService();

 private:
  
};