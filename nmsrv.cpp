

#include <csignal>
#include <iostream>
#include <unistd.h>
#include "ethnm.h"

EthnmCore* nmcore_ptr;
void handle_signal(int _signal) {
  if (nmcore_ptr != nullptr && (_signal == SIGINT || _signal == SIGTERM)) {
    printf("handle_signal exit!\n");
    nmcore_ptr->End();
  }
}

int main(int argc, char** argv) {
  uint32_t nm_state = 0;
  EthnmCore nmcore;
  nmcore_ptr = &nmcore;

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  nmcore.Init();
  nmcore.Start();

  printf("EthNM core exit!\n");
  return 0;
}