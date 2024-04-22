#ifndef __CONNECTION_MANAGER__
#define __CONNECTION_MANAGER__
#include <pthread.h>

#include "nm_util.h"

class TcpServer;
// 2024-03-25 ispark: Ethnm add
class Ethnm;

using UdpPacketReceived = void (*)(uint8_t*, uint32_t, void*);

class ConnectionManager {
 public:
  TcpServer* server;
  // 2024-03-25 ispark: ethnm add
  Ethnm* ethnm;

  int reciever_sock_udp;
  int sender_sock_udp;

  void* ethnm_context;

  ConnectionManager(TcpServer* server);

  // 2024-03-25 ispark: ethnm add
  ConnectionManager(Ethnm* ethnm);

  ~ConnectionManager();

  UdpPacketReceived udp_received;
  void RegisterUdpPacket(UdpPacketReceived, void* context);

  void SetUdpSocket();

  void RecieveMulticast();
  void SendMulticast();

  // udp thread
  void StartUdpReceiveThread();
  void StopUdpReceiveThread();

  void StartThread();
  void StartThreadInternal();

  void Stop();
  void StopThread();

 private:
  int fd_; /* socket file descriptor */

  pthread_t* thread_;
  pthread_t* udp_packet_receive_;
  void ErrorBreak(const char* s);
};
#endif /* __CONNECTION_MANAGER__ */