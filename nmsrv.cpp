#include <csignal>
#include <iostream>
#include <unistd.h>
#include "ethnm.h"

EthnmCore* nmcore_ptr;
void handle_signal(int _signal) {
  if (nmcore_ptr != nullptr && (_signal == SIGINT || _signal == SIGTERM)) {
    std::cout << "handle_signal exit!\n" << std::endl;
    nmcore_ptr->End();
  }
}

int main(int argc, char** argv) {
  std::string state("state");
  uint32_t nm_state = init_state;

  int i = 1;
  while (i < argc) {
    if (state == argv[i]) {
      nm_state = std::atoi(argv[i + 1]);
    }
    i++;
  }

  // EthnmCore* nmcore = new EthnmCore(nm_state, init_state);

  // uint32_t nm_state = 0;
  // TcpServer* srv1 = new TcpServer("127.0.0.1", 40000, "tcp server1");
  // printf("state : %d\n", nm_state);
  EthnmCore* ethnm = new EthnmCore(nm_state, init_state);
  nmcore_ptr = ethnm;

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  ethnm->Init();
  ethnm->Start();

  std::cout << "EthNM core exit!\n" << std::endl;
  return 0;
}