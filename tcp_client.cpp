#include "tcp_client.h"

TcpClient::TcpClient(TcpServer* srv, uint32_t ip, uint16_t port, int fd) {
  this->srv = srv;
  this->ip = ip;
  this->port = port;
  this->fd = fd;
}