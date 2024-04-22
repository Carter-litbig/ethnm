#ifndef __NM_UTIL__
#define __NM_UTIL__

#include <cstdint>

#define MULTICAST_ADDR "239.0.127.255"
#define MULTICAST_PORT 13800

#define DUT_ID 126
#define DUT_MAC "02:00:00:00:20:00"

#define TESTER_ID 127
#define TESTER_MAC "02:00:00:00:40:00"

#define BUFSIZE 512
#define TTL_DEFAULT 1
#define DEFAULT_OPTION 1

#define WAIT_SLEEP_TIME 5
#define WAIT_SLEEP_INTERVAL 10

struct NmPacket {
  unsigned char data[BUFSIZE];
  int data_len;
  uint32_t data_size;
  uint32_t data_seq;
  bool is_finish;
};

#endif /* __NM_UTIL__ */