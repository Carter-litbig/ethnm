#include "client_service.h"
#include "tcp_server.h"

ClientService::ClientService(TcpServer* srv) { this->tcp_srv = srv; }

ClientService::~ClientService() {}