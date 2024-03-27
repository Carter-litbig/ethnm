#include <assert.h>
#include "msg_delimiter.h"
#include "msg_delimiter_fixed.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "ByteCircularBuffer.h"

MsgDelimiterFixed::MsgDelimiterFixed(uint16_t size) : MsgDelimiter(kBcbSize) {
  // MsgDelimiter(kBcbSize);
  // 위와 같이 사용하면 에러 발생
  // cannot declare variable ‘kBcbSize’ to be of abstract type ‘MsgDelimiter’
  this->fixed_size = size;
}

MsgDelimiterFixed::~MsgDelimiterFixed() {}

bool MsgDelimiterFixed::IsFlushable() {
  if ((this->MsgDelimiter::bcb->current_size / this->fixed_size) > 0) {
    return true;
  }

  return false;
}

void MsgDelimiterFixed::Destroy() { this->MsgDelimiter::Destroy(); }

void MsgDelimiterFixed::ProcessMsg(TcpClient* client) {
  uint16_t read_size;

  if (!this->IsFlushable()) return;

  while (read_size = BCBRead(this->MsgDelimiter::bcb, this->MsgDelimiter::buf,
                             this->fixed_size, true)) {
    client->server->received(client->server, client, this->MsgDelimiter::buf,
                             read_size);
  }
}
