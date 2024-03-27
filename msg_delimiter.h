#pragma once

#include <stdint.h>

const int kBcbSize = 10240;

enum class MsgSizeType {
  kMsgSizeNone,
  kMsgSizeFixed,
  kMsgSizeVariable,
  kMsgSizePattern,
};

typedef struct ByteCircularBuffer_ ByteCircularBuffer;
class TcpClient;

class MsgDelimiter {
 private:
 protected:
  ByteCircularBuffer* bcb;
  unsigned char* buf;

 public:
  virtual bool IsFlushable() = 0;
  virtual void ProcessMsg(TcpClient*) = 0;

  MsgDelimiter(uint16_t buf_len);
  MsgDelimiter();
  ~MsgDelimiter();

  void Destroy();
  void Process(TcpClient*, unsigned char* msg, uint16_t size);
};