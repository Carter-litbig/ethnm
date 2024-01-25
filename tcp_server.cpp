#include <stdio.h>
#include <assert.h>
#include "tcp_server.h"
#include "network_utils.h"

TcpServer::TcpServer(std::string ip, uint16_t port, std::string name) {
  this->ip = network_covert_ip_p_to_n(ip.c_str());
  this->port = port;
  this->name = name;

  this->connection_manager_ = new ConnectionManager(this);
  this->client_db_ = new ClientDB(this);
  this->client_service_ = new ClientService(this);
}

void TcpServer::Start() {
  /* Start connection manager */
  this->connection_manager_->StartThread();

  /* Start client service */
  this->client_service_->StartThread();

  /* Init client db */
  this->client_db_->Create();

  printf("tcp server Start [%s, %d]\nok\n",
         network_convert_ip_n_to_p(this->ip, 0), this->port);
}

void TcpServer::Stop() {}

void TcpServer::AddClient(TcpClient* client) {
  this->client_db_->Update(client);
  this->client_service_->Listen(client);
}

void TcpServer::RegisterListener(ClientConnected connected,
                                 ClientDisconnected disconnected,
                                 ClientReceived received) {
  this->connected = connected;
  this->disconnected = disconnected;
  this->received = received;
}

void TcpServer::Display() {
  printf("server name: %s\n", this->name.c_str());
  printf("listening on: [%s, %d]\n", network_convert_ip_n_to_p(this->ip, 0),
         this->port);

  this->client_db_->Display();
}