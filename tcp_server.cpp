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

void TcpServer::start() {
  /* Start connection manager */
  this->conn_mgr_->startThread();

  /* Start client service */
  this->cli_svc_->startThread();

  /* Init client db */
  this->cli_db_->create();

  printf("tcp server start [%s, %d]\nok\n",
         network_convert_ip_n_to_p(this->ip, 0), this->port);
}

void TcpServer::stop() {}

void TcpServer::updateClient(TcpClient* client) {
  this->cli_db_->update(client);
  this->cli_svc_->listen(client);
}

void TcpServer::registerListener(ClientConnected connected,
                                 ClientDisconnected disconnected,
                                 ClientReceived received) {
  this->connected = connected;
  this->disconnected = disconnected;
  this->received = received;
}