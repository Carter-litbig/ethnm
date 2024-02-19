#include "tcp_client.h"
#include <stdio.h>
#include <arpa/inet.h>
#include "network_utils.h"

TcpClient::TcpClient(TcpServer* server, uint32_t ip, uint16_t port, int fd) {
  this->server = server;
  this->ip = ip;
  this->port = port;
  this->fd = fd;
}

void TcpClient::Display() { 

  printf("tcp client: [%s, %d] connected to [%s, %d]\n",
         network_convert_ip_n_to_p(htonl(this->ip), 0), (this->port),
         network_convert_ip_n_to_p(htonl(this->srv_ip), 0), (this->srv_port));
}