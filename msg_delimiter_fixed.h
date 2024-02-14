#pragma once

#include <stdint.h>

class TcpClient;

class MsgDelimiterFixed : public MsgDelimiter {
 private:
  uint16_t fixed_size;

 public:
  bool IsFlushable() override;
  void ProcessMsg(TcpClient* client) override;

  MsgDelimiterFixed(uint16_t size);
  ~MsgDelimiterFixed();
  void Destroy();
};