#pragma once
// tcpsrv
#include <stdint.h>
#include <string>

#include "connection_manager.h"
#include "client_db.h"
#include "client_service.h"

#include "tcp_client.h"

/* Notify for application */
#if 0
typedef void (*ClientConnected)(const TcpServer*, const TcpClient);
typedef void (*ClientDisconnected)(const TcpServer*, const TcpClient);
// const TcpServer*, const TcpClient, unsigned char* msg, uint16_t len
typedef void (*ClientReceived)(const TcpServer*, const TcpClient,
                               unsigned char*, uint16_t);
#endif

using ClientConnected = void (*)(const TcpServer*, const TcpClient*);
using ClientDisconnected = void (*)(const TcpServer*, const TcpClient*);
using ClientReceived = void (*)(const TcpServer*, const TcpClient*,
                                unsigned char*, uint16_t);

class TcpServer {
 public:
  uint32_t ip;
  uint16_t port;
  std::string name;

  TcpServer(std::string ip, uint16_t port, std::string name);
  ~TcpServer();

  void start();
  void stop();

  void updateClient(TcpClient* client);

  ClientConnected connected;
  ClientDisconnected disconnected;
  ClientReceived received;

  /* Register listeners for application */
  void registerListener(ClientConnected, ClientDisconnected, ClientReceived);

  void display();

 private:
  ConnectionManager* conn_mgr_;
  ClientDB* cli_db_;
  ClientService* cli_svc_;
};