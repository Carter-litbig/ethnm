#pragma once
// tcpsrv
#include <stdint.h>
#include <string>

#include "connection_manager.h"
#include "client_db.h"
#include "client_service.h"

#include "tcp_client.h"

class TcpServer {
 public:
  uint32_t ip;
  uint16_t port;
  std::string name;

  TcpServer(std::string ip, uint16_t port, std::string name);
  ~TcpServer();

  void start();
  void stop();

  void updateClient(TcpClient* client);

 private:
  ConnectionManager* conn_mgr_;
  ClientDB* cli_db_;
  ClientService* cli_svc_;
};