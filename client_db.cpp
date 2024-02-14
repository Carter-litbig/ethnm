#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer* server) { this->server = server; }

ClientDB::~ClientDB() {}

void ClientDB::Create() {}

void ClientDB::Update(TcpClient* client) { this->clients_.push_back(client); }

void ClientDB::Display() {
  std::list<TcpClient*>::iterator it;
  TcpClient* client;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    client->Display();
  }
}
