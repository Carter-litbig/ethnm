#pragma once
// tcpsrv
#include <stdint.h>
#include <string>

class TcpServer {
 public:
  uint32_t ip;
  uint16_t port;
  std::string name;

  TcpServer(std::string ip, uint16_t port, std::string name);

  void start();
  void stop();

 private:
  ConnectionManager* conn_mgr_;
  ClientDB* cli_db_;
  ClientService* cli_svc_;
}