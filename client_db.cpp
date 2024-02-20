#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer *server) { this->server = server; }

ClientDB::~ClientDB() {}

void ClientDB::Create() {}

void ClientDB::Update(TcpClient *client) { this->clients_.push_back(client); }

void ClientDB::Display() {
  std::list<TcpClient *>::iterator it;
  TcpClient *client;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    client->Display();
  }
}

void ClientDB::Copy(std::list<TcpClient *> *list) {
  std::list<TcpClient *>::iterator it;
  TcpClient *client;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    list->push_back(client);
  }
}

void ClientDB::Purge() {
  std::list<TcpClient *>::iterator it;
  TcpClient *client, *next_client;

  for (it = this->clients_.begin(), client = *it; it != this->clients_.end();
       client = next_client) {
    next_client = *(it++);

    this->clients_.remove(client);
    client->Abort();
  }
}
