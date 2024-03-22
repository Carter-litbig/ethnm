#include "ethnm.h"
#include "connection_manager.h"

void EthnmCore::Init(void) {
    static int ecu_id;

    ecu_id = DUT_ID;
    state_var = wake_up;

}

void EthnmCore::Open() {
  ConnectionManager->SetUdpSocket();

}

void EthnmCore::Start() {
    this->StartThread();

}

void EthnmCore::StartThread() {
    if (pthread_create(this->thread, nullptr, EthNmThread,
                       (void *)this))
    {
        printf("%s() thread creation failed, error=%d\n", __FUNCTION__, errno);
        exit(0);
    }

  printf("service started: EthNM_Thread:\n");
}

void EthnmCore::SendNmMsg() {
    ConnectionManager->SendMulticast();
}

void EthnmCore::RecieveNmMSg() {
    ConnectionManager->RecieveMulticast();

}

void EthnmCore::SetNmState() {

}

void EthnmCore::GetNmState() {

}

void EthnmCore::StopThread() {

}

void EthnmCore::Close() {

}

void EthnmCore::End() {

}

void EthnmCore::Sleep() {

}

void EthnmCore::Notify() {

}




void error_break(char *s) {
  perror(s);
  exit(1);
}

static void *EthNmThread(void *arg) {
  EthnmCore->SendNmMsg();
  EthnmCore->RecieveNmMSg();
}

static void *StateManagerThread(void *arg) {

}

static int Paser(int packet) {
  int buf[BUFSIZE + 1];
  buf


}