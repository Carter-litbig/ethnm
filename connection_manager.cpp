#include <iostream>
#include <chrono>  // sleep_for
#include <memory.h>
#include <cstring>  //std::memset
#include <thread>   //std::this_thread
#include <sys/socket.h>
#include <netinet/in.h>  // for IPPROTO_TCP
#include <arpa/inet.h>
#include <unistd.h>  // for close() socket

#include "network_utils.h"
#include "tcp_client.h"
#include "msg_delimiter.h"
#include "msg_delimiter_fixed.h"
#include "connection_manager.h"
#include "tcp_server.h"

#include "ethnm.h"

ConnectionManager::ConnectionManager(TcpServer *server) {
  this->server = server;

  this->fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->fd_ < 0) {
    printf("error: could not Create accept fd\n");
    exit(0);
  }

  this->thread_ = (pthread_t *)calloc(1, sizeof(pthread_t));
}

// 2024-03-25 ispark: ConnectionManager add
ConnectionManager::ConnectionManager(Ethnm *ethnm) {
  this->ethnm = ethnm;
}

ConnectionManager::~ConnectionManager() {}

/* static 에서는 private fd 접근이 안되어 이의 접근을 위해 별도로 만듦 */
void ConnectionManager::StartThreadInternal() {
  int opt = 1;
  struct sockaddr_in s_addr;  // server address

  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(this->server->port);
  s_addr.sin_addr.s_addr = htonl(this->server->ip);

  if (setsockopt(this->fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                 sizeof(opt)) < 0) {
    printf("setsockopt failed\n");
    exit(0);
  }

  if (setsockopt(this->fd_, SOL_SOCKET, SO_REUSEPORT, (char *)&opt,
                 sizeof(opt)) < 0) {
    printf("setsockopt failed\n");
    exit(0);
  }

  /* Bind the socket */
  if (bind(this->fd_, (struct sockaddr *)&s_addr, sizeof(struct sockaddr)) ==
      -1) {
    printf("error: connmgr socket bind failed[%s(0x%x), %d], error=%d",
           network_convert_ip_n_to_p(this->server->ip, 0), this->server->ip,
           this->server->port, errno);
    exit(0);
  }

  if (listen(this->fd_, 5) < 0) {
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
    fd = accept(this->fd_, (struct sockaddr *)&c_addr, &c_addr_len);

    if (fd < 0) {
      printf("error in accepting new connections\n");
      continue;
    }

    TcpClient *client = new TcpClient(this->server, c_addr.sin_addr.s_addr,
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

void ConnectionManager::SetUdpSocket() {
  int sent_recv_bytes;
  // int ttl;

  int ttl = TTL_DEFAULT;
  int option = DEFAULT_OPTION;

  // struct sockaddr_in server_addr;
  // struct sockaddr_in client_addr;
  struct sockaddr_in multicast_addr;
  struct ip_mreq mreq;

  if ((reciever_sock_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    this->ErrorBreak("set UDP(reveive)");
  }

  //----------------------------------------------------------------------
  // 메시지 수신을 위한 UDP 소켓 생성
  sender_sock_udp = socket(AF_INET, SOCK_DGRAM, /*IPPROTO_UDP*/ 0);
  if (sender_sock_udp < 0) {
    this->ErrorBreak("set UDP(sender)");
  }

  if (setsockopt(sender_sock_udp, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl,
                 sizeof(ttl)) < 0) {
    this->ErrorBreak("ttl");
  }
  // 주소 재사용 설정
  if (setsockopt(reciever_sock_udp, SOL_SOCKET, SO_REUSEADDR, (void *)&option,
                 sizeof(option)) < 0) {
    this->ErrorBreak("Failed to set reuseaddr!");
  }
  //----------------------------------------------------------------------

  memset((char *)&multicast_addr, 0, sizeof(multicast_addr));
  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_port = htons(MULTICAST_PORT);
  multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(reciever_sock_udp, (struct sockaddr *)&multicast_addr,
           sizeof(struct sockaddr)) < 0) {
    this->ErrorBreak("socket");
  }

  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(reciever_sock_udp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq,
                 sizeof(mreq)) < 0) {
    this->ErrorBreak("join muticast group");
  }
}

void ConnectionManager::RecieveMulticast() {
  Packet rcvpkt;
  // sleep check
  static uint32_t T_WaitBusSleep = 0;  // count
  static bool nm_msg_check = false;
  memset(&rcvpkt, 0, sizeof(rcvpkt));

  while (1) {
    // std::cout << "[received data]\n" << std::endl;
    std::memset(&rcvpkt, 0, sizeof(rcvpkt));
    rcvpkt.data_len = recvfrom(reciever_sock_udp, rcvpkt.data, BUFSIZE, 0, NULL,
                               0);  // 브로드캐스트 된 데이터를 수신
    if (rcvpkt.data_len < 0) {
      ErrorBreak("multicast receive");
    } else {
      rcvpkt.data[rcvpkt.data_len] = '\0';
      // sleep check
      if ((rcvpkt.data_len == 8u && rcvpkt.data[0] != 0x7F) &&
          (nm_msg_check == false)) {
        T_WaitBusSleep++;
        // sleep count: 5s
        if (T_WaitBusSleep > WAIT_SLEEP_TIME) {
          this->ethnm->state_var = READY_SLEEP;  // send msg stop
          this->ethnm->send_msg_running = false;
          T_WaitBusSleep = 0;
          nm_msg_check = true;
        }
      } else {
        nm_msg_check = false;
        T_WaitBusSleep = 0;
      }
      this->ethnm->Parser((uint8_t *)rcvpkt.data, rcvpkt.data_len);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void ConnectionManager::SendMulticast() {
  uint8_t msg[] = {0x7E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  struct sockaddr_in send_addr;
  memset(&send_addr, 0, sizeof(send_addr));
  send_addr.sin_family = AF_INET;
  send_addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
  send_addr.sin_port = htons(MULTICAST_PORT);

  while (this->ethnm->send_msg_running) {
    if (ethnm->state_var == REPEAT_MESSAGE || ethnm->state_var == NORMAL_OPERATION) {
      if (sendto(sender_sock_udp, msg, sizeof(msg), 0, (sockaddr *)&send_addr,
                 sizeof(send_addr)) < 0) {
        this->ErrorBreak("send multicast msg");
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

static void *ConnectionManagerThread(void *arg) {
  ConnectionManager *cm = (ConnectionManager *)arg;

  cm->StartThreadInternal();
  return 0;
}

void ConnectionManager::StartThread() {
  /* Start a thread_ */
  if (pthread_create(this->thread_, nullptr, ConnectionManagerThread,
                     (void *)this)) {
    printf("%s() thread_ creation failed, error=%d\n", __FUNCTION__, errno);
    exit(0);
  }
}

void ConnectionManager::StopThread() {
  if (!this->thread_) return;

  printf("pthread_cancel\n");
  pthread_cancel(*this->thread_);

  /* Wait until the thread_ is cancelled successfully */
  pthread_join(*this->thread_, NULL);

  free(this->thread_);
  this->thread_ = NULL;
}

void ConnectionManager::Stop() {
  // /* 1. Stop the connmgr thread_ if it is running */
  // this->StopThread();

  // /* 2. Release the resource (accept_fd) */
  // close(this->fd);
  // this->fd = 0;

  // /* 3. Delete this instance of connmgr */
  // delete this;
  close(reciever_sock_udp);
  close(sender_sock_udp);
}

void ConnectionManager::ErrorBreak(const char *s) {
  perror(s);
  exit(1);
}