#pragma once
// connmgr

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* tcp_srv;

  ConnectionManager(TcpServer* srv);

 private:
}