#include <csignal>
#include <chrono>
#include <iostream>
#include <cstring>
#include <sstream>    // std::ostringstream
#include <algorithm>  // std::copy
#include <iterator>   // std::ostream_iterator
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ethnm_ids.hpp"
#include "ethnm_tester.hpp"

uint32_t receiver_sock;
uint32_t sender_sock;

EthnmTester::EthnmTester(uint32_t _message_number)
    : message_number_(_message_number),
      thread_running_(true),
      sender_running_(false),
      receiver_running_(false),
      nm_message_check_(false),
      sender_thread_(std::bind(&EthnmTester::SenderThread, this)),
      receiver_thread_(std::bind(&EthnmTester::ReceiverThread, this)) {}

EthnmTester::~EthnmTester() {}
bool EthnmTester::Init() {
  if (Open() == false) {
    std::cout << "UDP socket open failure!" << std::endl;
  }
  MakeMsg();

  return true;
}

bool EthnmTester::Open(void) {
  int ttl = 1;
  int option = 1;
  sockaddr_in localAddr;

  // 멀티캐스트 주소와 포트 설정
  std::memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(PORT);

  // 메시지 송신을  위한 UDP 소켓 생성
  sender_sock = socket(AF_INET, SOCK_DGRAM, /*IPPROTO_UDP*/ 0);
  if (receiver_sock < 0) {
    std::cerr << "Failed to sender socket\n";
    return false;
  }
  // 멀티캐스트 패킷의 TTL 설정, 성공하면 sentto 가능
  if (setsockopt(sender_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&ttl,
                 sizeof(ttl)) < 0) {
    std::cerr << "Failed to set TTL\n";
    return false;
  }

  // 메시지 수신을 위한 UDP 소켓 생성
  receiver_sock = socket(AF_INET, SOCK_DGRAM, /*IPPROTO_UDP*/ 0);
  if (receiver_sock < 0) {
    std::cerr << "Failed to receive socket\n";
    return false;
  }

  // 주소 재사용 설정
  if (setsockopt(receiver_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option,
                 sizeof(option)) < 0) {
    std::cerr << "Failed to set reuseaddr!\n";
    return false;
  }

  /* 멀티캐스트 주소정보를 기반으로 주소할당 */
  if (bind(receiver_sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) <
      0) {
    std::cerr << "Failed to bind to local port\n";
    return -1;
  }

  // 멀티캐스트 그룹 가입을 위한 구조체
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(receiver_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&mreq,
                 sizeof(mreq)) < 0) {
    std::cerr << "Failed to join multicast group\n";
    return false;
  }
  return true;
}

void EthnmTester::Close() {
  close(receiver_sock);
  close(sender_sock);
}

void EthnmTester::ReceiverThread() {
  int str_len = 0;
  unsigned char buf[BUFSIZE] = "";
  std::memset(&buf, 0, sizeof(buf));

  // test
  struct sockaddr_in sender_addr;
  socklen_t sender_addr_len = sizeof(sender_addr);

  //  멀티 그룹 전체에 보내기 때문에 자신의 메시지도 수신
  while (receiver_running_) {
    str_len =
        recvfrom(receiver_sock, buf, BUFSIZE, 0, (struct sockaddr*)&sender_addr,
                 &sender_addr_len);  // 브로드캐스트 된 데이터를 수신
    if (str_len > 0u) {
      buf[str_len] = '\0';
      DecodeMsg(buf, sender_addr, str_len);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void EthnmTester::SenderThread() {
  sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
  addr.sin_port = htons(PORT);

  // 주기적으로 메시지 전송
  while (thread_running_) {
    while (sender_running_) {
      // "Received message!" 메시지 전송
      if (SendMsg(&msg_vecter_[message_number_][0], addr,
                  msg_vecter_[message_number_].size()) < 0) {
        std::cerr << "Failed to send message\n";
        Stop();
      }
      if (message_number_ != 0) {
        sender_running_ = false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
}
void EthnmTester::MakeMsg() {
  msg_vecter_ = {{
                     // nm msg
                     0x7F,
                     0x01,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                 },
                 {
                     // startup_request_msg 0x01,
                     0x05, 0x01, 0x33, 0x12, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
                 },
                 {
                     // release_request_msg
                     0x05, 0x01, 0x33, 0x51, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
                 }};
}

// udp 메시지 전송
uint32_t EthnmTester::SendMsg(std::uint8_t* msg, sockaddr_in addr,
                              uint32_t size) {
  uint32_t result = 1;
  result = sendto(sender_sock, msg, size, 0, (sockaddr*)&addr, sizeof(addr));

  return result;
}

// 송신측 포트번호 Display
void EthnmTester::GetSenderPort(sockaddr_in addr) {
  // 송신 측의 포트 번호 확인
  unsigned short sender_port = ntohs(addr.sin_port);
  std::cout << "Sender port: " << sender_port << std::endl;
}

// command prompt process
void EthnmTester::CommandProcess() {
  int command_number;
  sleep(1);
PROMPT_USER:

  /* prompt the user to enter data */
  // printf("Select number: 1-PLAY/2-STOP/3-RESUME/4-QUIT\n");
  std::cout << "[ Select number ]\n"
            << "1-NM message send(repeat)\n"
            << "2-NM message stop\n"
            << "3-TriggerNetworkStartupRequest call\n"
            << "4-TriggerNetworkReleaseRequest call\n"
            << "5-Quit\n"
            << std::endl;

  // 1-NM message send(repeat)
  // 2-NM message stop
  // 3-TriggerNetworkStartupRequest call
  // 4-TriggerNetworkReleaseRequest call
  std::cin >> command_number;

  // 잘못된 입력을 받았을 경우
  if (std::cin.fail()) {
    if (std::cin.eof()) {
      goto PROMPT_EXIT;
    }
    // 에러 메시지 출력
    std::cout << "Wrong Number. Retry!" << std::endl;
    // 오류스트림을 초기화
    std::cin.clear();
    // 입력버퍼를 비움
    std::cin.ignore(256, '\n');
    goto PROMPT_USER;
  }

  switch (command_number) {
    // NM message send(repeat)
    case NM_TEST_MSG_REPEAT:
      std::cout << "NM message send(repeat)" << std::endl;
      message_number_ = NM_TEST_MSG_REPEAT - 1;
      sender_running_ = true;
      break;
    case NM_TEST_MSG_STOP:
      std::cout << "NM message stop" << 2 << std::endl;
      sender_running_ = false;
      nm_message_check_ = false;
      break;
    case NM_TEST_DUT_START:
      std::cout << "TriggerNetworkStartupRequest call" << std::endl;
      message_number_ = NM_TEST_DUT_START - 2;
      sender_running_ = true;

      break;
    case NM_TEST_DUT_RELEASE:
      std::cout << "TriggerNetworkReleaseRequest call" << std::endl;
      sender_running_ = true;
      nm_message_check_ = false;
      message_number_ = NM_TEST_DUT_RELEASE - 2;
      break;
    case NM_TEST_QUIT:
      goto PROMPT_EXIT;
      break;
    default:
      break;
  }
  goto PROMPT_USER;

PROMPT_EXIT:
  if (thread_running_ == true) {
    Stop();
  }
  return;
}

// received msg parsing
void EthnmTester::DecodeMsg(std::uint8_t* msg, sockaddr_in addr,
                            uint32_t size) {
  static uint32_t count = 0u;
  uint16_t service_id = 0;
  uint8_t service_primitive_id = 0;
#if 1
  if ((size == 8u && msg[0] == 0x7E) && (nm_message_check_ == false)) {
    count++;
    if (count > 3u) {
      std::cout << "NM message received\n" << std::endl;
      nm_message_check_ = true;
      count = 0;
      // port 번호(13800)는 wireshark으로 확인
      // GetSenderPort(addr);
    }
  }
#else
  // DUT 에서 command 파싱
  if (size == 20) {
    std::memcpy(&service_id, &msg[0], sizeof(uint16_t));
    service_primitive_id = msg[3];
    if (service_id == 0x0105u && service_primitive_id == 0x12u) {
      std::cout << "NM message repeat start!\n" << std::endl;
    } else if (service_id == 0x0105u && service_primitive_id == 0x51u) {
      std::cout << "NM_STATE sleep\n" << std::endl;
    }
  }

  for (uint8_t i = 0; i < size; ++i) {
    std::cout << "0x" << std::hex << (int)msg[i] << ",";
  }
  // if (size == 20) {
  //   std::cout << service_id << service_primitive_id;
  // }
  std::cout << std::endl;
#endif
}

void EthnmTester::Start() {
  sender_running_ = false;
  receiver_running_ = true;

  CommandProcess();
}

void EthnmTester::Stop() {
  sender_running_ = false;
  receiver_running_ = false;
  thread_running_ = false;

  // socket close
  Close();

  if (std::this_thread::get_id() != sender_thread_.get_id()) {
    if (sender_thread_.joinable()) {
      sender_thread_.join();
    }
  } else {
    sender_thread_.detach();
  }
  if (std::this_thread::get_id() != receiver_thread_.get_id()) {
    if (receiver_thread_.joinable()) {
      receiver_thread_.join();
    }
  } else {
    receiver_thread_.detach();
  }
}

EthnmTester* tester_ptr(nullptr);
void handle_signal(int _signal) {
  if (tester_ptr != nullptr && (_signal == SIGINT || _signal == SIGTERM)) {
    tester_ptr->Stop();
    std::cout << "\nhandle_signal exit!\n" << std::endl;
  }
}

int main(int argc, char** argv) {
  uint32_t nm_state = 0;
  EthnmTester tester(nm_state);
  tester_ptr = &tester;

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);
  if (tester.Init()) {
    tester.Start();
    std::cout << "ethnm tester exit!" << std::endl;
    return 0;
  } else {
    return 1;
  }
}