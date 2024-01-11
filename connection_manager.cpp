#include "connection_manager.h"
#include "tcp_server.h"

ConnectionManager::ConnectionManager(TcpServer* srv) { this->srv = srv; }

ConnectionManager::~ConnectionManager() {}

void ConnectionManager::startThread() {}
