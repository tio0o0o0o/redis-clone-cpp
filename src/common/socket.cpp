#include "../../include/socket.h"
#include <cstring>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Socket::~Socket() {
  if (fd != -1)
    close(fd);
}

int Socket::send(int fd, std::string buffer) {
  return ::send(fd, buffer.c_str(), buffer.length(), 0);
}

std::string Socket::read(int fd, size_t size) {
  std::string buffer(size, '\0');
  ::read(fd, buffer.data(), size);
  return buffer;
}
