#include "client_db.h"
#include "tcp_server.h"

ClientDB::ClientDB(TcpServer* srv) { this->tcp_srv = srv; }

ClientDB::~ClientDB() {}