#include <pthread.h>
#include "nm_util.h"

typedef enum {
  init_state,
  wake_up,
  preapre_sleep,
  repeat_message,
  normal_op,
  ready_sleep,
  sleep_bus,
  error_state
} nm_state_t;
class ConnectionManager;
class EthnmCore {
 public:
  int state_var;
  uint8_t ethnm_packet[BUFSIZE];
  EthnmCore(int state_var_, int pre_state_var_);
  ~EthnmCore();
  void Init();
  void Open();
  void Start();
  void Close();
  void End();
  void SetNmState();
  void StartThread();
  void SendNmMsg();
  void RecieveNmMSg();
  void GetNmState(int stat);
  void Sleep();
  void StopThread();
  void Stop();
  void Notify();
  void error_break(const char* s);

 private:
  // 2024-03-26 ispark: connection_manager_ add.
  ConnectionManager* connection_manager_;
  int pre_state_var;
  nm_state_t nm_state;

  // pthread_t* nmthread;
  pthread_t* tid_network;
  pthread_t* tid_statemanager;
};

static int Parser(void);