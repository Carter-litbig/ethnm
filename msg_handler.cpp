#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "msg_handler.h"
#include "ByteCircularBuffer.h"
#include "tcp_client.h"

MsgHandler::MsgHandler(uint16_t buf_len) {
  this->bcb = BCBCreateNew(buf_len);
  this->buf = (unsigned char*)calloc(kBufferSize, sizeof(unsigned char));
}

MsgHandler::MsgHandler() {
  this->bcb = BCBCreateNew(kBcbSize);
  this->buf = (unsigned char*)calloc(kBufferSize, sizeof(unsigned char));
}

MsgHandler::~MsgHandler() {
  assert(!this->bcb);
  assert(!this->buf);
}

void MsgHandler::Destroy() {
  if (this->bcb) {
    BCBFree(this->bcb);
    this->bcb = nullptr;
  }

  if (this->buf) {
    free(this->buf);
    this->buf = nullptr;
  }
}

void MsgHandler::Process(TcpClient* client, unsigned char* msg, uint16_t size) {
  assert(BCBWrite(client->msg_handler->bcb, msg, size));

  if (!this->IsFlushable()) return;

  // this->ProcessMsg(client);
}