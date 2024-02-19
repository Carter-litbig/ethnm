#include "connection_manager.h"
#include "tcp_server.h"

#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>  // for IPPROTO_TCP

#include "network_utils.h"

#include "tcp_client.h"
#include "msg_delimiter.h"
#include "msg_delimiter_fixed.h"

ConnectionManager::ConnectionManager(TcpServer* server) {
  this->server = server;

  this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->fd < 0) {
    printf("error: could not Create accept fd\n");
    exit(0);
  }

  this->thread = (pthread_t*)calloc(1, sizeof(pthread_t));
}

ConnectionManager::~ConnectionManager() {}

/* static 에서는 private fd 접근이 안되어 이의 접근을 위해 별도로 만듦 */
void ConnectionManager::StartThreadInternal() {
  int opt = 1;
  struct sockaddr_in s_addr;  // server address

  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(this->server->port);
  s_addr.sin_addr.s_addr = htonl(this->server->ip);

  if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) <
      0) {
    printf("setsockopt failed\n");
    exit(0);
  }

  if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, sizeof(opt)) <
      0) {
    printf("setsockopt failed\n");
    exit(0);
  }

  /* Bind the socket */
  if (bind(this->fd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr)) ==
      -1) {
    printf("error: connmgr socket bind failed[%s(0x%x), %d], error=%d",
           network_convert_ip_n_to_p(this->server->ip, 0), this->server->ip,
           this->server->port, errno);
    exit(0);
  }

  if (listen(this->fd, 5) < 0) {
    printf("Listen failed\n");
    exit(0);
  }

  /* Create an infinite loop */
  struct sockaddr_in c_addr;  // client address
  socklen_t c_addr_len = sizeof(c_addr);
  int fd;

  /* 서버 소켓은 한개만 열어두고 여러 개의 클라이언트 수락 가능. 서버 옵션 참조
   */
  while (true) {
    /* Invoke accept() to acept new connections */
    fd = accept(this->fd, (struct sockaddr*)&c_addr, &c_addr_len);

    if (fd < 0) {
      printf("error in accepting new connections\n");
      continue;
    }

    TcpClient* client = new TcpClient(this->server, c_addr.sin_addr.s_addr,
                                      c_addr.sin_port, fd);

    /* assignment_6 */
    client->srv_ip = this->server->ip;
    client->srv_port = this->server->port;

    if (this->server->connected != nullptr) {
      this->server->connected(this->server, client);
    }

    client->msg_delimiter = new MsgDelimiterFixed(27);

    /* Update Client DB */
    this->server->AddClient(client);

    printf("connection accepted from client [%s, %d]\n",
           network_convert_ip_n_to_p(htonl(c_addr.sin_addr.s_addr), 0),
           htons(c_addr.sin_port));

    /* Notify the application for new connections */
  }
}

static void* ConnectionManagerThread(void* arg) {
  ConnectionManager* cm = (ConnectionManager*)arg;

  cm->StartThreadInternal();
}

void ConnectionManager::StartThread() {
  /* Start a thread */
  if (pthread_create(this->thread, nullptr, ConnectionManagerThread,
                     (void*)this)) {
    printf("%s() thread creation failed, error=%d\n", __FUNCTION__, errno);
    exit(0);
  }

  printf("service started: ConnectionManagerThread:\n");
}
