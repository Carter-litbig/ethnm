#include <csignal>
#include <iostream>
#include <unistd.h>
#include "ethnm.h"

Ethnm* ethnm_ptr;
void handle_signal(int _signal) {
  if (ethnm_ptr != nullptr && (_signal == SIGINT || _signal == SIGTERM)) {
    std::cout << "handle_signal exit!\n" << std::endl;
    ethnm_ptr->Stop();
  }
}

int main(int argc, char** argv) {
  std::string state("state");
  uint32_t nm_state = INIT_STATE;

  int i = 1;
  while (i < argc) {
    if (state == argv[i]) {
      nm_state = std::atoi(argv[i + 1]);
    }
    i++;
  }

  // ./nmsrv state value(0,2,4,8,16,32,64)
  Ethnm* ethnm = new Ethnm(nm_state, INIT_STATE);
  ethnm_ptr = ethnm;

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  ethnm->Init();
  ethnm->Start();

  scanf("\n");
  std::cout << "EthNM core exit!\n" << std::endl;
  return 0;
}