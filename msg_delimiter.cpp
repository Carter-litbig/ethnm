#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "msg_delimiter.h"
#include "ByteCircularBuffer.h"
#include "udp_client.h"

MsgDelimiter::MsgDelimiter(uint16_t buf_len) {
  this->bcb = BCBCreateNew(buf_len);
  this->buf = (unsigned char*)calloc(kBufferSize, sizeof(unsigned char));
}

MsgDelimiter::MsgDelimiter() {
  this->bcb = BCBCreateNew(kBcbSize);
  this->buf = (unsigned char*)calloc(kBufferSize, sizeof(unsigned char));
}

MsgDelimiter::~MsgDelimiter() {
  assert(!this->bcb);
  assert(!this->buf);
}

void MsgDelimiter::Destroy() {
  if (this->bcb) {
    BCBFree(this->bcb);
    this->bcb = nullptr;
  }

  if (this->buf) {
    free(this->buf);
    this->buf = nullptr;
  }
}

void MsgDelimiter::Process(TcpClient* client, unsigned char* msg, uint16_t size) {
  assert(BCBWrite(client->msg_delimiter->bcb, msg, size));

  if (!this->IsFlushable()) return;

  this->ProcessMsg(client);
}