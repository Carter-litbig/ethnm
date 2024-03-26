#include <pthread.h>
#include "nm_util.h"

typedef enum {
  init,
  wake_up,
  preapre_sleep,
  repeat_message,
  normal,
  ready_sleep,
  sleep_bus,
  error
} nm_state_t;

class EthnmCore {
 public:
  uint8_t ethnm_packet[BUFSIZE];
  EthnmCore();
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
  void Notify();

 private:
  int state_var;
  nm_state_t nm_state;
};

static int Parser(void);