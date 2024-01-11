#include "tcp_server.h"

int main(int argc, char** argv) {
  TcpServer* srv1 = new TcpServer("127.0.0.1", 40000, "tcp server1");

  srv1->start();
  scanf("\n");
  return 0;
}