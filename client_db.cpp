#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer* srv) { this->srv = srv; }

ClientDB::~ClientDB() {}

void ClientDB::init() {}