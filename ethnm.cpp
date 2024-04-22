#include <iostream>
#include <unistd.h>
#include <sys/stat.h>  //mkfifo
#include <cstring>     //std::strlen
#include <fcntl.h>     //open

#include "nm_util.h"
#include "ethnm.h"
#include "connection_manager.h"

// 생성자
Ethnm::Ethnm(int s, int ps) : state_var_(s), pre_state_var_(ps) {
  connection_manager_ = new ConnectionManager(this);
  pthread_rwlock_init(&this->rwlock_, NULL);

  this->tid_network_ = (pthread_t *)calloc(1, sizeof(pthread_t));
  this->tid_statemanager_ = (pthread_t *)calloc(1, sizeof(pthread_t));
}

// 소멸자
Ethnm::~Ethnm() {}

void Ethnm::Init() {
  // this->Start();
}

/*************************************************************
 static function
**************************************************************/
static void *EthNmThread(void *arg) {
  Ethnm *ethnm = (Ethnm *)arg;
  int state_mask = (REPEAT_MESSAGE || NORMAL_OPERATION || READY_SLEEP);
  while (true) {
    if ((ethnm->GetNmState() & state_mask)) {
      ethnm->RecieveNmMSg();
    } else {
      // exit(0);
    }
  }
}

static void *StateManagerThread(void *arg) {
  Ethnm *ethnm = (Ethnm *)arg;
  while (true) {
    ethnm->NmStateNotify();
    ethnm->SendNmMsg();
  }
}

int Ethnm::Parser(uint8_t *pkt, uint32_t len) {
  uint16_t service_id = 0;
  uint8_t service_primitive_id = 0;

  if (len > 0) {
    if (len == 8u && pkt[0] == 0x7F) {
      SetNmState(REPEAT_MESSAGE);
    } else if (len == 20u) {
      std::memcpy(&service_id, &pkt[0], sizeof(uint16_t));
      service_primitive_id = pkt[3];

      if (service_id == 0x0105u && service_primitive_id == 0x12u) {
        SetNmState(REPEAT_MESSAGE);
      } else if (service_id == 0x0105u && service_primitive_id == 0x51u) {
        SetNmState(SLEEP_BUS);
      }
    }
  }
  return 0;
}

void Ethnm::Open() { connection_manager_->SetUdpSocket(); }

void Ethnm::Start() {
  Open();
  StartThread();
}

void Ethnm::StartThread() {
  if (pthread_create(this->tid_network_, nullptr, EthNmThread, (void *)this) <
      0) {
    ErrorBreak("thread create");
  }

  if (pthread_create(tid_statemanager_, nullptr, StateManagerThread,
                     (void *)this) < 0) {
    ErrorBreak("thread create");
  }
}

void Ethnm::SendNmMsg() { connection_manager_->SendMulticast(); }

void Ethnm::RecieveNmMSg() { connection_manager_->RecieveMulticast(); }

void Ethnm::NmStateNotify() {
  if (pre_state_var_ != GetNmState()) {
    pre_state_var_ = GetNmState();

    if (GetNmState() == INIT_STATE) {
      std::cout << "Init state\n" << std::endl;
    } else if (GetNmState() == WAKE_UP) {
      std::cout << "Wake up done\n" << std::endl;
    } else if (GetNmState() == PREPARE_SLEEP) {
      std::cout << "Prepare sleep\n" << std::endl;
    } else if (GetNmState() == REPEAT_MESSAGE) {
      std::cout << "Go repeat message\n" << std::endl;
    } else if (GetNmState() == NORMAL_OPERATION) {
      std::cout << "Go normal operation\n" << std::endl;
    } else if (GetNmState() == SLEEP_BUS) {
      std::cout << "Go sleep\n" << std::endl;
      SleepNotify();
    } else if (GetNmState() == READY_SLEEP) {
      std::cout << "Go READY_SLEEP\n" << std::endl;
    } else {
      std::cout << "State Errror!\n" << std::endl;
    }
  }
}
int Ethnm::GetNmState() {
  pthread_rwlock_rdlock(&this->rwlock_);
  return state_var_;
  pthread_rwlock_unlock(&this->rwlock_);
}
void Ethnm::SetNmState(int state) {
  pthread_rwlock_wrlock(&this->rwlock_);
  state_var_ = state;
  pthread_rwlock_unlock(&this->rwlock_);
}

void Ethnm::Close() { connection_manager_->Stop(); }

void Ethnm::End() {
  SetNmState(INIT_STATE);
  pre_state_var_ = INIT_STATE;
}

void Ethnm::Sleep() {}

void Ethnm::SleepNotify() {
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
    ErrorBreak("pipe open\n");
  }

  // 메시지 전송
  if (write(fd, message, std::strlen(message)) != std::strlen(message)) {
    ErrorBreak("pipe write\n");
    close(fd);
  }
  // 파이프 닫기
  close(fd);
}

void Ethnm::ErrorBreak(const char *s) {
  perror(s);
  exit(1);
}

void Ethnm::StopThread() {
  if ((!this->tid_network_) || (!this->tid_statemanager_)) return;

  pthread_cancel(*this->tid_network_);
  pthread_cancel(*this->tid_statemanager_);

  /* Wait until the thread is cancelled successfully */
  pthread_join(*this->tid_network_, NULL);
  pthread_join(*this->tid_statemanager_, NULL);

  free(this->tid_network_);
  free(this->tid_statemanager_);

  this->tid_network_ = NULL;
  this->tid_statemanager_ = NULL;
}
void Ethnm::Stop() {
  this->StopThread();

  connection_manager_->Stop();
  delete connection_manager_;
  delete this;
}