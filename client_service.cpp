#include "client_service.h"
#include "tcp_server.h"

ClientService::ClientService(TcpServer* srv) { this->srv = srv; }

ClientService::~ClientService() {}