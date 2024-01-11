#pragma once
// connmgr

class TcpServer;

class ConnectionManager {
 public:
  TcpServer* srv;

  ConnectionManager(TcpServer* srv);
  ~ConnectionManager();

 private:
};