#include "connection_manager.h"
#include "tcp_server.h"

ConnectionManager::ConnectionManager(TcpServer* srv) { this->tcp_srv = srv; }

ConnectionManager::~ConnectionManager() {}