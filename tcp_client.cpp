#include "tcp_client.h"
#include <stdio.h>
#include <arpa/inet.h>
#include "network_utils.h"

TcpClient::TcpClient(TcpServer* srv, uint32_t ip, uint16_t port, int fd) {
  this->srv = srv;
  this->ip = ip;
  this->port = port;
  this->fd = fd;
}

void TcpClient::display() {
  printf("tcp client: [%s , %d]\n", network_convert_ip_n_to_p(htonl(this->ip), 0),
         (this->port));
}