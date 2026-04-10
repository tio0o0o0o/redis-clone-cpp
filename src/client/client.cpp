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

  std::cout << "getaddrinfo_res: " << getaddrinfo_res << '\n';

  fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  std::cout << "fd: " << fd << '\n';

  int connect_res = connect(fd, res->ai_addr, res->ai_addrlen);

  std::cout << "connect_res: " << connect_res << '\n';

  freeaddrinfo(res);

  return fd;
}

int Client::send(std::string buffer) { return Socket::send(fd, buffer); }

std::string Client::read(size_t size) { return Socket::read(fd, size); }
