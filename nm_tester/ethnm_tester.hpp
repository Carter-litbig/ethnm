#include <thread>
#include <vector>

class EthnmTester {
 public:
  EthnmTester(uint32_t _message_number);
  ~EthnmTester();
  bool Init();
  void Stop();
  bool Open();
  void Close();
  void Start();
  void MakeMsg();
  uint32_t SendMsg(std::uint8_t* msg, sockaddr_in addr, uint32_t size);
  void DecodeMsg(std::uint8_t* msg, sockaddr_in addr, uint32_t size);
  void GetSenderPort(sockaddr_in addr);
  void CommandProcess();
  void ReceiverThread();
  void SenderThread();

 private:
  std::vector<std::vector<uint8_t>> msg_vecter_;
  uint32_t message_number_;
  bool thread_running_;
  bool sender_running_;
  bool receiver_running_;
  bool nm_message_check_;
  std::thread receiver_thread_;
  std::thread sender_thread_;
};