#include <iostream>
#include <unistd.h>
#include <sys/stat.h>  //mkfifo
#include <cstring>     //std::strlen
#include <fcntl.h>     //open

#include "nm_util.h"
#include "ethnm.h"
#include "connection_manager.h"

// 생성자
Ethnm::Ethnm(int s, int ps)
    : state_var(s),
      pre_state_var_(ps),
      nm_state_(INIT_STATE),
      send_msg_running(true) {
      connection_manager_ = new ConnectionManager(this);

  this->tid_network_ = (pthread_t *)calloc(1, sizeof(pthread_t));
  this->tid_statemanager_ = (pthread_t *)calloc(1, sizeof(pthread_t));
}

// 소멸자
Ethnm::~Ethnm() { Stop(); }

void Ethnm::Init() {
  nm_state_e nm_stat;

  // this->Start();
}

/*************************************************************
 static function
**************************************************************/
static void *EthNmThread(void *arg) {
  Ethnm *ethnm = (Ethnm *)arg;
  while (true) {
    if (ethnm->state_var == REPEAT_MESSAGE || ethnm->state_var == NORMAL_OPERATION ||
        ethnm->state_var == READY_SLEEP) {
      ethnm->RecieveNmMSg();
    } else {
      // exit(0);
    }
  }
}

static void *StateManagerThread(void *arg) {
  Ethnm *ethnm = (Ethnm *)arg;
  while (true) {
    ethnm->SetNmState();
    ethnm->SendNmMsg();
  }
}

int Ethnm::Parser(uint8_t *pkt, uint32_t len) {
  uint16_t service_id = 0;
  uint8_t service_primitive_id = 0;

  if (len > 0) {
    if (len == 8u && pkt[0] == 0x7F) {
      state_var = REPEAT_MESSAGE;
    } else if (len == 20u) {
      std::memcpy(&service_id, &pkt[0], sizeof(uint16_t));
      service_primitive_id = pkt[3];

      if (service_id == 0x0105u && service_primitive_id == 0x12u) {
        state_var = REPEAT_MESSAGE;
      } else if (service_id == 0x0105u && service_primitive_id == 0x51u) {
        state_var = SLEEP_BUS;
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

  pthread_join(*this->tid_network_, NULL);
  pthread_join(*this->tid_statemanager_, NULL);
}

void Ethnm::SendNmMsg() { connection_manager_->SendMulticast(); }

void Ethnm::RecieveNmMSg() { connection_manager_->RecieveMulticast(); }

void Ethnm::SetNmState() {
  if (pre_state_var_ != state_var) {
    pre_state_var_ = state_var;

    if (state_var == WAKE_UP) {
      std::cout << "Wake up done\n" << std::endl;
    } else if (state_var == REPEAT_MESSAGE) {
      std::cout << "Go repeat message\n" << std::endl;
      send_msg_running = true;
    } else if (state_var == NORMAL_OPERATION) {
      std::cout << "Go normal operation\n" << std::endl;
    } else if (state_var == SLEEP_BUS) {
      std::cout << "Go sleep\n" << std::endl;
      send_msg_running = false;
      Notify();

    } else if (state_var == READY_SLEEP) {
      std::cout << "Go READY_SLEEP\n" << std::endl;
      send_msg_running = false;
    } else if (state_var == ERROR_STATE) {
      std::cout << "State Errror!\n" << std::endl;
    }
  }
}

void Ethnm::Close() { connection_manager_->Stop(); }

void Ethnm::End() {
  state_var = INIT_STATE;
  pre_state_var_ = INIT_STATE;
}

void Ethnm::Sleep() {}

void Ethnm::Notify() {
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