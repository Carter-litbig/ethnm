#include <assert.h>
#include "msg_delimiter.h"
#include "msg_delimiter_var.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "ByteCircularBuffer.h"

const int kHeaderMsgSize = 2;

MsgDelimiterVar::MsgDelimiterVar() : MsgDelimiter(kBcbSize) {
  // MsgDelimiter(kBcbSize);
  // 위와 같이 사용하면 에러 발생
  // cannot declare variable ‘kBcbSize’ to be of abstract type ‘MsgDelimiter’
}

MsgDelimiterVar::~MsgDelimiterVar() {}

bool MsgDelimiterVar::IsFlushable() {
  uint16_t msg_size;
  ByteCircularBuffer_t* bcb = this->MsgDelimiter::bcb;

  if (bcb->current_size <= kHeaderMsgSize) return false;

  BCBRead(bcb, (unsigned char*)&msg_size, kHeaderMsgSize, false);

  if (msg_size <= bcb->current_size) return true;

  return false;
}

void MsgDelimiterVar::Destroy() { this->MsgDelimiter::Destroy(); }

void MsgDelimiterVar::ProcessMsg(TcpClient* client) {
  uint16_t read_size;
  ByteCircularBuffer_t* bcb = this->MsgDelimiter::bcb;

  while (this->IsFlushable()) {
    BCBRead(bcb, (unsigned char*)&read_size, kHeaderMsgSize, false);
    assert(read_size == BCBRead(bcb, this->MsgDelimiter::buf, read_size, true));

    client->server->received(client->server, client, this->MsgDelimiter::buf,
                             read_size);
  }
}
