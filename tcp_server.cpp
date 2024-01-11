#include <stdio.h>
#include <assert.h>
#include "tcp_server.h"
#include "network_utils.h"

TcpServer::TcpServer(std::string ip, uint16_t port, std::string name) {
  this->ip = network_covert_ip_p_to_n(ip.c_str());
  this->port = port;
  this->name = name;

  this->conn_mgr_ = new ConnectionManager(this);
  this->cli_db_ = new ClientDB(this);
  this->cli_svc_ = new ClientService(this);
}

void TcpServer::start() {}

void TcpServer::stop() {}