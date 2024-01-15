#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer* srv) { this->srv = srv; }

ClientDB::~ClientDB() {}

void ClientDB::create() {}

void ClientDB::update(TcpClient* client) { this->clients_.push_back(client); }

void ClientDB::display() {
  std::list<TcpClient*>::iterator it;
  TcpClient* cli;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    cli = *it;
    cli->display();
  }
}
