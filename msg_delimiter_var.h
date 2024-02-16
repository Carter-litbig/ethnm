#pragma once

#include <stdint.h>

const int kVarBufSize = 256;

class TcpClient;

class MsgDelimiterVar : public MsgDelimiter {

 public:
  bool IsFlushable() override;
  void ProcessMsg(TcpClient* client) override;

  MsgDelimiterVar();
  ~MsgDelimiterVar();
  void Destroy();
};