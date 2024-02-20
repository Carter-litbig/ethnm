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

  this->SetBit(TCP_SERVER_INITIALIZED);
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

  this->SetBit(TCP_SERVER_RUNNING);
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

  if (!this->IsBitSet(TCP_SERVER_RUNNING)) {
    printf("tcp server not running\n");
    return;
  }

  printf("listening on: [%s, %d]\n", network_convert_ip_n_to_p(this->ip, 0),
         this->port);

  printf("flags:  ");

  if (this->IsBitSet(TCP_SERVER_INITIALIZED)) {
    printf("I");
  } else {
    printf("Un-I");
  }

  if (this->IsBitSet(TCP_SERVER_RUNNING)) {
    printf(" R");
  } else {
    printf(" Not-R");
  }

  if (this->IsBitSet(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)) {
    printf(" Not-Acc");
  } else {
    printf(" Acc");
  }

  if (this->IsBitSet(TCP_SERVER_NOT_LISTENING_CLIENTS)) {
    printf(" Not-L");
  } else {
    printf(" L");
  }

  if (this->IsBitSet(TCP_SERVER_CREATE_MULTI_THREADED_CLIENT)) {
    printf(" M");
  } else {
    printf(" Not-M");
  }

  printf("\n");

  this->client_db_->Display();
}

void TcpServer::SetBit(uint32_t bit) { this->state |= bit; }

void TcpServer::UnSetBit(uint32_t bit) { this->state &= ~bit; }

bool TcpServer::IsBitSet(uint32_t bit) { return (this->state & bit); }

void TcpServer::StopConnectionAcceptance() {
  printf("%s() called\n", __FUNCTION__);
  if (this->IsBitSet(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)) return;

  printf("SetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)\n");
  this->SetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS);
  this->connection_manager_->Stop();
  this->connection_manager_ = NULL;
}

void TcpServer::StartConnectionAcceptance() {
  printf("%s() called\n", __FUNCTION__);
  if (!this->IsBitSet(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS)) return;
  this->UnSetBit(TCP_SERVER_NOT_ACCEPTING_NEW_CONNECTIONS);
  this->connection_manager_ = new ConnectionManager(this);
  this->connection_manager_->StartThread();
}