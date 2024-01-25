#pragma once
// tcpcli
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

class MsgHandler {
 private:
 protected:
  ByteCircularBuffer* bcb;
  unsigned char* buf;

 public:
  virtual bool IsFlushable() = 0;
  virtual void ProcessMsg() = 0;

  MsgHandler(uint16_t buf_len);
  MsgHandler();
  ~MsgHandler();

  void Destroy();
  void Process(TcpClient*, unsigned char* msg, uint16_t size);
};