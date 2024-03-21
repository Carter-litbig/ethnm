#include "udp_client.h"
#include <stdio.h>
#include <arpa/inet.h>
#include "network_utils.h"
#include <unistd.h>  // close socket
#include "msg_delimiter.h"

UdpClient::UdpClient(TcpServer* server, uint32_t ip, uint16_t port, int fd) {
  this->server = server;
  this->ip = ip;
  this->port = port;
  this->fd = fd;
}

void UdpClient::Display() {
  printf("udp client: [%s, %d] connected to [%s, %d]\n",
         network_convert_ip_n_to_p(htonl(this->ip), 0), (this->port),
         network_convert_ip_n_to_p(htonl(this->srv_ip), 0), (this->srv_port));
}

void UdpClient::Abort() {
  if (this->fd) {
    close(this->fd);
    this->fd = 0;
  }

  this->server = nullptr;

  if (this->msg_delimiter) {
    this->msg_delimiter->Destroy();
    delete this->msg_delimiter;
    this->msg_delimiter = nullptr;
  }

  delete this;
}