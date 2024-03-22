#include <pthread.h>

#define DUT_ID 125
#define DUT_PORT

#define TESTER_ID 126
#define TESTER_PORT 


typedef enum {
  init,
  wake_up,
  preapre_sleep,
  repeat_message,
  normal,
  ready_sleep,
  sleep_bus,
  error

}nm_state_t;

class EthnmCore {
    static int state_var;
  void Init();
  void Open();
  void Start();
  void Close();
  void End();
  void SetNmState();


 public :
  void StartThread();
  void SendNmMsg();
  void RecieveNmMSg();
  void GetNmState();
  void StopThread();
  void Notify();

  nm_state_t nm_state;
};