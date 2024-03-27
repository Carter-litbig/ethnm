#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>  // for sleep

#include "client_service.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "msg_delimiter.h"

#include <assert.h>

unsigned char data[kBufferSize];

ClientService::ClientService(TcpServer *server) {
  this->server = server;
  this->max_fd = 0;
  FD_ZERO(&this->active_fd_set);
  FD_ZERO(&this->backup_fd_set);
  thread = (pthread_t *)calloc(1, sizeof(pthread_t));
}

ClientService::~ClientService() {}

int ClientService::GetMaxFd() {
  int max_fd_lcl = 0;

  TcpClient *client;
  std::list<TcpClient *>::iterator it;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    if (client->fd > max_fd_lcl) {
      max_fd_lcl = client->fd;
    }
  }
  return max_fd_lcl;
}

void ClientService::CopyClientFd(fd_set *fdset) {
  TcpClient *client;
  std::list<TcpClient *>::iterator it;

  for (it = this->clients_.begin(); it != this->clients_.end(); ++it) {
    client = *it;
    FD_SET(client->fd, fdset);
  }
}

void ClientService::StartThreadInternal() {
  /* Invoke select system call on all clients present in client db */
  int data_len;
  TcpClient *client, *next_client;
  struct sockaddr_in addr;
  std::list<TcpClient *>::iterator it;

  socklen_t addr_len = sizeof(addr);

  /* assignment_7_2 */
  if (this->server->IsBitSet(TCP_SERVER_NOT_LISTENING_CLIENTS)) {
    this->server->CopyClients(&this->clients_);
  }

  /* Copy all clients fd */
  this->max_fd = this->GetMaxFd();
  FD_ZERO(&this->backup_fd_set);
  this->CopyClientFd(&this->backup_fd_set);

  while (true) {
    memcpy(&this->active_fd_set, &this->backup_fd_set, sizeof(fd_set));
    select(this->max_fd + 1, &this->active_fd_set, 0, 0, 0);

    for (it = this->clients_.begin(), client = *it; it != this->clients_.end();
         client = next_client) {
      next_client = *(++it);

      if (FD_ISSET(client->fd, &this->active_fd_set)) {
        data_len = recvfrom(client->fd, data, kBufferSize, 0,
                            (struct sockaddr *)&addr, &addr_len);
      }

      if (data_len == 0) {
        printf("recvfrom: data_len 0, error no = %d\n", errno);
        sleep(1);
      }

      if (client->msg_delimiter) {
        client->msg_delimiter->Process(client, data, data_len);
      } else if (this->server->received) {
        this->server->received(this->server, client, data, data_len);
      }
    }
    memset(data, 0, data_len);
  }
}

void *ClientServiceThread(void *arg) {
  ClientService *cs = (ClientService *)arg;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);

  cs->StartThreadInternal();
  return nullptr;
}

void ClientService::StartThread() {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(this->thread, &attr, ClientServiceThread, (void *)this);

  /* Note that, even if you implement all steps above correctly, yet your TCP
   * Server do not reponds to client's data requests , the problem could be
   * resolved by inserting a sleep(1) as below: */

  // resolved race condition:
  // https://www.youtube.com/watch?v=lN6psOV5M9o
  sleep(1);
  printf("service started: ClientServiceThread\n");
}

void ClientService::StopThread() {
  pthread_cancel(*this->thread);
  pthread_join(*this->thread, nullptr);
  free(this->thread);
  this->thread = nullptr;
}

void ClientService::AddClient(TcpClient *client) {
  this->clients_.push_back(client);
}

/* If client service thread is blocked on select(), we cannot modify
 * active_fd_set since it being used by select(). If service thread is
 * servicing clients in a for loop, we cannot modify the active_fd_set since
 * it is being read by client service thread (read - write conflict) */
void ClientService::Listen(TcpClient *client) {
  /* Connection manager therad cancels the client service thread at cancellation
   * points(pthread_cancel).
   * Connection manager waits for the cancellation to complete (pthread_join)
   * Connection manager updates client DBs
   * Connection manager restart the client service thread
   */
  this->StopThread();
  printf("client service thread is cancelled\n");

  this->AddClient(client);

  this->thread = (pthread_t *)calloc(1, sizeof(pthread_t));
  this->StartThread();
}

void ClientService::Stop() {
  /* 1. Cancel client service thread */
  this->StopThread();

  /* 2. Cleanup client db 'of client service' */
  std::list<TcpClient *>::iterator it;
  TcpClient *client, *next_client;

  /* this function assumes that client service thread is already cancelled,
   * hence no need to lock anything */
  assert(this->thread == NULL);

  for (it = this->clients_.begin(), client = *it; it != this->clients_.end();
       client = next_client) {
    next_client = *(it++);
    this->clients_.remove(client);
    /* Note that these clients are still present in client db */
  }

  /* 3. Delete client service altogether */
  delete this;
}
