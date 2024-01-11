#include "connection_manager.h"
#include "tcp_server.h"

#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h> // for IPPROTO_TCP

ConnectionManager::ConnectionManager(TcpServer* srv) {
  this->srv = srv;
  
  this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->fd < 0) {
    printf("error: could not create accept fd\n");
    exit(0);
  }

  this->thread = (pthread_t*)calloc(1, sizeof(pthread_t));
}

ConnectionManager::~ConnectionManager() {}

void ConnectionManager::startThread() {}
