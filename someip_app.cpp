#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

int main() {
  // 네임드 파이프 열기 (읽기 전용)
  const char *fifo_path = "/tmp/nmpipe";
  int fd = open(fifo_path, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  // 메시지 수신
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
  if (bytes_read == -1) {
    perror("read");
    close(fd);
    return -1;
  }

  // 수신한 메시지 출력
  std::cout << "Received message in someip_app process: " << buffer
            << std::endl;

  // 파이프 닫기
  close(fd);

  return 0;
}