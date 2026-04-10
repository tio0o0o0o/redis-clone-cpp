#include "../../include/client.h"
#include "../../include/logger.h"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int Client::create() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res;

  int getaddrinfo_res = getaddrinfo(ip.empty() ? "127.0.0.1" : ip.c_str(),
                                    port.c_str(), &hints, &res);

  if (getaddrinfo_res == -1) {
    LOG_ERROR("Failed to run getaddrinfo");
    return -1;
  }

  fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (fd == -1) {
    LOG_ERROR("Failed to create socket");
    return -1;
  }

  int connect_res = connect(fd, res->ai_addr, res->ai_addrlen);

  if (connect_res == -1) {
    LOG_ERROR("Failed to connect");
    return -1;
  }

  freeaddrinfo(res);

  return fd;
}

int Client::send(std::string buffer) { return Socket::send(fd, buffer); }

std::string Client::read(size_t size) { return Socket::read(fd, size); }
