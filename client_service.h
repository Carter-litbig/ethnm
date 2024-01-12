#pragma once
// clisvc

class TcpClient;
class TcpServer;

class ClientService {
 public:
  TcpServer* srv;

  ClientService(TcpServer* srv);
  ~ClientService();

  void startThread();
  void listen(TcpClient* client);

 private:
  
};