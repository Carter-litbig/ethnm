#include "tcp_server.h"
#include "tcp_client.h"
#include "network_utils.h"
#include <arpa/inet.h>

static void printClient(const TcpClient* cli) {
  printf("[%s, %d]\n", network_convert_ip_n_to_p(htonl(cli->ip), 0),
         htons(cli->port));
}

static void printServer(const TcpServer* srv) {
  printf("[%s, %d]\n", network_convert_ip_n_to_p(srv->ip, 0), htons(srv->port));
}

static void connectedListener(const TcpServer* srv, const TcpClient* cli) {
  printServer(srv);
  printf("client connected: ");
  printClient(cli);
}

static void disconnectedListener(const TcpServer* srv, const TcpClient* cli) {}

static void receivedListener(const TcpServer*, const TcpClient*,
                             unsigned char* msg, uint16_t len) {
  printf("%s() bytes received: %d, msg: %s\n", __FUNCTION__, len, msg);
}

int main(int argc, char** argv) {
  TcpServer* srv1 = new TcpServer("127.0.0.1", 40000, "tcp server1");

  srv1->registerListener(connectedListener, disconnectedListener,
                         receivedListener);

  srv1->start();
  scanf("\n");

  srv1->display();
  return 0;
}