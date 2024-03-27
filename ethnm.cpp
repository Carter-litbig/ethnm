#include <iostream>
#include <unistd.h>
#include <sys/stat.h>  //mkfifo
#include <cstring>     //std::strlen
#include <fcntl.h>     //open

#include "nm_util.h"
#include "ethnm.h"
#include "connection_manager.h"

// 생성자
EthnmCore::EthnmCore(int state_var_, int pre_state_var_)
    : state_var(state_var_),
      pre_state_var(pre_state_var_),
      nm_state(init_state) {
  connection_manager_ = new ConnectionManager(this);

  this->tid_network = (pthread_t *)calloc(1, sizeof(pthread_t));
  this->tid_statemanager = (pthread_t *)calloc(1, sizeof(pthread_t));
}

// 소멸자
EthnmCore::~EthnmCore() {Stop();}

void EthnmCore::Init() {
  static int ecu_id;
  nm_state_t nm_stat;
  ecu_id = DUT_ID;

  // this->Start();
}

/*************************************************************
 static function
**************************************************************/
static void *EthNmThread(void *arg) {
  EthnmCore *nmcore = (EthnmCore *)arg;
  while (true) {
    if (nmcore->state_var == repeat_message || nmcore->state_var == normal_op) {
      nmcore->SendNmMsg();
      nmcore->RecieveNmMSg();
      nmcore->GetNmState(Parser());
    } else {
      // exit(0);
    }
  }
  // EthnmCore->StopThread(ETHNM_NETWORK_THREAD_ID);
  // pthread_join(nmcore->tid_network, NULL);
  // mcore->Close();
  // return 0;
}

static void *StateManagerThread(void *arg) {
  EthnmCore *nmcore = (EthnmCore *)arg;
  while (true) {
    nmcore->SetNmState();
    // EthnmCore->Notify();
  }
  // return 0;
}

static int Parser(void) {}

void EthnmCore::Open() { connection_manager_->SetUdpSocket(); }

void EthnmCore::Start() {
  Open();
  state_var = wake_up;
  StartThread();
}

void EthnmCore::StartThread() {
  if (pthread_create(this->tid_network, nullptr, EthNmThread, (void *)this) <
      0) {
    // printf("%s() thread creation failed, error=%d\n", __FUNCTION__, errno);
    error_break("thread create");
  }

  printf("service started: EthNM_Thread\n");

  if (pthread_create(tid_statemanager, nullptr, StateManagerThread,
                     (void *)this) < 0) {
    // printf("%s() thread creation failed, error=%d\n", __FUNCTION__, errno);
    error_break("thread create");
  }
  printf("service started: State_Manager\n");

  pthread_join(*this->tid_network, NULL);
  pthread_join(*this->tid_statemanager, NULL);
}

void EthnmCore::SendNmMsg() { connection_manager_->SendMulticast(); }

void EthnmCore::RecieveNmMSg() { connection_manager_->RecieveMulticast(); }

void EthnmCore::SetNmState() {
  if (pre_state_var != state_var) {
    pre_state_var = state_var;

    if (state_var == wake_up) {
      printf("Wake up done\n");
      // EthnmCore->Notify(wake_up);
    } else if (state_var == repeat_message) {
      printf("Go repeat message\n");
      // EthnmCore->Notify(repeat_message);
    } else if (state_var == normal_op) {
      printf("Go normal operation\n");
      // EthnmCore->Notify(normal_op);
    } else if (state_var == sleep_bus) {
      printf("Go sleep\n");
      Stop();
      // EthnmCore->Notify(sleep_bus);
    } else if (state_var == error_state) {
      printf("State Errror!\n");
      // EthnmCore->Notify(error_state);
    }
  }
}

void EthnmCore::GetNmState(int stat) {
  switch (stat) {
    case init_state:
      state_var = init_state;
      break;
    case wake_up:
      state_var = wake_up;
      break;
    case preapre_sleep:
      state_var = preapre_sleep;
      break;
    case repeat_message:
      state_var = repeat_message;
      break;
    case normal_op:
      state_var = normal_op;
      break;
    case ready_sleep:
      state_var = ready_sleep;
      break;
    case sleep_bus:
      state_var = sleep_bus;
      break;
    case error_state:
      state_var = error_state;
      break;
    default:
      break;
  }
}


void EthnmCore::Close() { connection_manager_->Stop(); }

void EthnmCore::End() {
  state_var = init_state;
  pre_state_var = init_state;
}

void EthnmCore::Sleep() {}

void EthnmCore::Notify() {
  const char *fifo_path = "/tmp/nmpipe";
  const char *message = "Sleep command!";

  if (access(fifo_path, F_OK) == -1) {
    std::string command = "mkfifo ";
    command += fifo_path;

    // 네임드 파이프 생성
    int status = system(command.c_str());

    if (status != 0) {
      std::cerr << "Error: Unable to create /tmp/nmpipe" << std::endl;
    }
    std::cout << "/tmp/nmpipe has been created successfully" << std::endl;
  } else {
    std::cout << "/tmp/nmpipe already exists" << std::endl;
  }

  mkfifo(fifo_path, 0666);

  // 파이프 열기 (쓰기 전용)
  int fd = open(fifo_path, O_WRONLY);
  if (fd == -1) {
    perror("open fail!\n");
  }

  // 메시지 전송
  if (write(fd, message, std::strlen(message)) != std::strlen(message)) {
    perror("write fail!\n");
    close(fd);
  }
  // 파이프 닫기
  close(fd);
}

void EthnmCore::error_break(const char *s) {
  perror(s);
  exit(1);
}

void EthnmCore::StopThread() {
  printf("StopThread\n");
  if ((!this->tid_network) || (!this->tid_statemanager)) return;

  pthread_cancel(*this->tid_network);
  pthread_cancel(*this->tid_statemanager);

  /* Wait until the thread is cancelled successfully */
  pthread_join(*this->tid_network, NULL);
  pthread_join(*this->tid_statemanager, NULL);

  free(this->tid_network);
  this->tid_statemanager = NULL;
}
void EthnmCore::Stop() { 
  this-> StopThread();
  delete this;
}