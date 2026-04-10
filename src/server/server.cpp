#include "../../include/server.h"
#include "../../include/logger.h"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int Server::create() {
  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int getaddrinfo_res = getaddrinfo(ip.empty() ? nullptr : ip.c_str(),
                                    port.c_str(), &hints, &res);

  std::cout << "getaddrinfo_res: " << getaddrinfo_res << '\n';

  fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  std::cout << "fd: " << fd << '\n';

  int bind_res = bind(fd, res->ai_addr, res->ai_addrlen);

  std::cout << "bind_res: " << bind_res << '\n';

  freeaddrinfo(res);

  int listen_res = listen(fd, backlog);

  LOG_INFO("Server listening");

  return fd;
}

int Server::accept() {
  int client_fd = ::accept(fd, NULL, NULL);

  return client_fd;
}
