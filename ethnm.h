#include <pthread.h>

#include "nm_util.h"

enum {
  INIT_STATE = (0x01 << 0),
  WAKE_UP = (0x01 << 1),
  PREPARE_SLEEP = (0x01 << 2),
  REPEAT_MESSAGE = (0x01 << 3),
  NORMAL_OPERATION = (0x01 << 4),
  READY_SLEEP = (0x01 << 5),
  SLEEP_BUS = (0x01 << 6)
};

class ConnectionManager;

class Ethnm {
 public:
  Ethnm(int s, int ps);
  ~Ethnm();
  void Init();
  void Open();
  void Start();
  void Close();
  void End();
  int GetNmState();
  void SetNmState(int state);
  void NmStateNotify();
  void StartThread();
  void SendNmMsg();
  void RecieveNmMSg();
  void Sleep();
  void StopThread();
  void Stop();
  void SleepNotify();
  void ErrorBreak(const char* s);
  int Parser(uint8_t* p, uint32_t len);

 private:
  // 2024-03-26 ispark: connection_manager_ add.
  int state_var_;
  int pre_state_var_;
  ConnectionManager* connection_manager_;
  pthread_t* tid_network_;
  pthread_t* tid_statemanager_;
  pthread_rwlock_t rwlock_;
};
