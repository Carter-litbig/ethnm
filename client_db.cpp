<<<<<<< HEAD
<<<<<<< HEAD
#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer* server) { this->server = server; }

ClientDB::~ClientDB() {}

void ClientDB::Create() {}

void ClientDB::Update(TcpClient* client) { this->clients_.push_back(client); }

void ClientDB::Display() {
  std::list<TcpClient*>::iterator it;
  TcpClient* cli;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    cli = *it;
    cli->Display();
  }
}
=======
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer *server) {
  this->server = server;
  pthread_rwlock_init(&this->rwlock, NULL);
}

ClientDB::~ClientDB() { pthread_rwlock_destroy(&this->rwlock); }

void ClientDB::Create() {}

void ClientDB::Update(TcpClient *client) {
  pthread_rwlock_wrlock(&this->rwlock);
  this->clients_.push_back(client);
  pthread_rwlock_unlock(&this->rwlock);
}

void ClientDB::Display() {
  std::list<TcpClient *>::iterator it;
  TcpClient *client;

  pthread_rwlock_rdlock(&this->rwlock);
  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    client->Display();
  }
  pthread_rwlock_unlock(&this->rwlock);
}

void ClientDB::Copy(std::list<TcpClient *> *list) {
  std::list<TcpClient *>::iterator it;
  TcpClient *client;

  pthread_rwlock_rdlock(&this->rwlock);
  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    list->push_back(client);
  }
  pthread_rwlock_unlock(&this->rwlock);
}

void ClientDB::Purge() {
  std::list<TcpClient *>::iterator it;
  TcpClient *client, *next_client;

  pthread_rwlock_rdlock(&this->rwlock);
  for (it = this->clients_.begin(), client = *it; it != this->clients_.end();
       client = next_client) {
    next_client = *(it++);

    this->clients_.remove(client);
    client->Abort();
  }
  pthread_rwlock_unlock(&this->rwlock);
}
<<<<<<< HEAD
>>>>>>> 7aae719 (add deleted file)
=======
>>>>>>> 7aae7196fac60570b89d7991884599e1ec7b55c9
