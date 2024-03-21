#include "connection_manager.h"
#include "tcp_server.h"

#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>  // for IPPROTO_TCP

#include "network_utils.h"

#include "udp_client.h"
#include "msg_delimiter.h"
#include "msg_delimiter_fixed.h"

#include <unistd.h> // for close() socket

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

    /* Message delimiter */
    // client->msg_delimiter = new MsgDelimiterFixed(27);
    client->msg_delimiter = NULL;

    /* Update Client DB */
    this->server->AddClient(client);

    printf("connection accepted from client [%s, %d]\n",
           network_convert_ip_n_to_p(htonl(c_addr.sin_addr.s_addr), 0),
           htons(c_addr.sin_port));

    /* Notify the application for new connections */
  }
}

void ConnectionManager::UdpManager() {
  int master_sock_udp_fd;
  int sent_recv_bytes;
  int addr_len;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  struct sockaddr_in multicast_addr;

  if ((master_sock_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    this->error_break("socket");
  }

  memset((char *)&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_len = sizeof(client_addr);

  if (bind(master_sock_udp_fd, (struct sockaddr *)&server_addr,
           sizeof(struct sockaddr)) == -1) {
    this->error_break("socket");
  }

  while (1)
  {
    printf("Waiting for data\n");
    fflush(stdout);

    if (sent_recv_bytes = recvfrom(master_sock_udp_fd, data_buffer, DATA_LEN, 0, (struct sockaddr*)&client_addr, &addr_len) == -1)
    {
      this->error_break("recvfrom()");
    }

    printf("Received packet from %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    printf("Data: %s\n", data_buffer);

    if (sendto(master_sock_udp_fd, data_buffer, sent_recv_bytes, 0, (struct sockaddr*)&client_addr, addr_len) == -1)
    {
      this->error_break("sendto()");
    }
    else
    {
      printf("echo cmpl : %s\n", data_buffer);
    }
  }

  close(master_sock_udp_fd);
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

void ConnectionManager::StopThread() {
  if (!this->thread) return;

  printf("pthread_cancel\n");
  pthread_cancel(*this->thread);

  /* Wait until the thread is cancelled successfully */
  pthread_join(*this->thread, NULL);
  
  free(this->thread);
  this->thread = NULL;
}

void ConnectionManager::Stop() {
  /* 1. Stop the connmgr thread if it is running */
  this->StopThread();

  /* 2. Release the resource (accept_fd) */
  close(this->fd);
  this->fd = 0;

  /* 3. Delete this instance of connmgr */
  delete this;
}

void ConnectionManager::error_break(char *s) {
  perror(s);
  exit(1);
}