#include <pthread.h>

#include "nm_util.h"

using enum {
  INIT_STATE,
  WAKE_UP,
  PREPARE_SLEEP,
  REPEAT_MESSAGE,
  NORMAL_OPERATION,
  READY_SLEEP,
  SLEEP_BUS,
  ERROR_STATE
} nm_state_e;

class ConnectionManager;

class Ethnm {
 public:
  int state_var;
  bool send_msg_running;
  Ethnm(int s, int ps);
  ~Ethnm();
  void Init();
  void Open();
  void Start();
  void Close();
  void End();
  void SetNmState();
  void StartThread();
  void SendNmMsg();
  void RecieveNmMSg();
  void Sleep();
  void StopThread();
  void Stop();
  void Notify();
  void ErrorBreak(const char* s);
  int Parser(uint8_t* p, uint32_t len);

 private:
  // 2024-03-26 ispark: connection_manager_ add.
  ConnectionManager* connection_manager_;
  int pre_state_var_;
  nm_state_e nm_state_;
  pthread_t* tid_network_;
  pthread_t* tid_statemanager_;
};
