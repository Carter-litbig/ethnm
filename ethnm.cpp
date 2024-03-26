#include <iostream>
#include "nm_util.h"
#include "ethnm.h"
#include <unistd.h>
#include "connection_manager.h"

// 생성작
EthnmCore::EthnmCore() {}

// 소멸자
EthnmCore::~EthnmCore() {}

void EthnmCore::Init(void) {}

void EthnmCore::Open() {}

void EthnmCore::Start() {}

void EthnmCore::StartThread() {}

void EthnmCore::SendNmMsg() {}

void EthnmCore::RecieveNmMSg() {}

void EthnmCore::SetNmState() {}

void EthnmCore::GetNmState(int stat) {}

void EthnmCore::StopThread() {}

void EthnmCore::Close() {}

void EthnmCore::End() {}

void EthnmCore::Sleep() {}

void EthnmCore::Notify() {}

void error_break(char *s) {}

static void *EthNmThread(void *arg) { return 0; }

static void *StateManagerThread(void *arg) { return 0; }

static int Parser(void) { return 0; }