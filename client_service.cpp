#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h> // for sleep

#include "client_service.h"
#include "tcp_server.h"
#include "tcp_client.h"

#define CLI_SVC_RCV_BUF_SIZE 1024
unsigned char rcv_buf[CLI_SVC_RCV_BUF_SIZE];

ClientService::ClientService(TcpServer *srv) {
  this->srv = srv;
  this->max_fd = 0;
  FD_ZERO(&this->active_fd_set);
  FD_ZERO(&this->backup_fd_set);
  thread = (pthread_t *)calloc(1, sizeof(pthread_t));
}

ClientService::~ClientService() {}

int ClientService::getMaxFd() {
  int max_fd_lcl = 0;

  TcpClient *cli;
  std::list<TcpClient *>::iterator it;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    cli = *it;
    if (cli->fd > max_fd_lcl) {
      max_fd_lcl = cli->fd;
    }
  }
  return max_fd_lcl;
}

void ClientService::copyClientFd(fd_set *fdset) {
  TcpClient *cli;
  std::list<TcpClient *>::iterator it;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    cli = *it;
    FD_SET(cli->fd, fdset);
  }
}

void ClientService::startThreadInternal() {
  /* Invoke select system call on all clients present in client db */
  int data_len;
  TcpClient *cli, *next_cli;
  struct sockaddr_in addr;
  std::list<TcpClient *>::iterator it;

  socklen_t addr_len = sizeof(addr);

  /* Copy all clients fd */
  this->max_fd = this->getMaxFd();
  FD_ZERO(&this->backup_fd_set);
  this->copyClientFd(&this->backup_fd_set);

  while (true) {
    memcpy(&this->active_fd_set, &this->backup_fd_set, sizeof(fd_set));
    select(this->max_fd + 1, &this->active_fd_set, 0, 0, 0);

    for (it = this->clients_.begin(), cli = *it; it != this->clients_.end();
         cli = next_cli) {
      next_cli = *(++it);

      if (FD_ISSET(cli->fd, &this->active_fd_set)) {
        data_len = recvfrom(cli->fd, rcv_buf, CLI_SVC_RCV_BUF_SIZE, 0,
                            (struct sockaddr *)&addr, &addr_len);
        printf("recvfrom: \n");
      }

      if (data_len == 0) {
        printf("recvfrom: data_len 0, error no = %d\n", errno);
        sleep(1);
      }

      if (this->srv->received) {
        this->srv->received(this->srv, cli, rcv_buf, data_len);
      }
    }
  }
}

void *client_service_thread(void *arg) {
  ClientService *cs = (ClientService *)arg;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  cs->startThreadInternal();
  return NULL;
}

void ClientService::startThread() {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(this->thread, &attr, client_service_thread, (void *)this);
  printf("service started: client_service_thread\n");
}

void ClientService::stopThread() {
  pthread_cancel(*this->thread);
  pthread_join(*this->thread, NULL);
  free(this->thread);
  this->thread = NULL;
}

void ClientService::addClient(TcpClient *client) {
  this->clients_.push_back(client);
}

/* If client service thread is blocked on select(), we cannot modify
 * active_fd_set since it being used by select(). If service thread is
 * servicing clients in a for loop, we cannot modify the active_fd_set since
 * it is being read by client service thread (read - write conflict) */
void ClientService::listen(TcpClient *client) {
  /* Connection manager therad cancels the client service thread at cancellation
   * points(pthread_cancel).
   * Connection manager waits for the cancellation to complete (pthread_join)
   * Connection manager updates client DBs
   * Connection manager restart the client service thread
   */
  this->stopThread();
  printf("client service thread is cancelled\n");

  this->addClient(client);

  this->thread = (pthread_t *)calloc(1, sizeof(pthread_t));
  this->startThread();
}
