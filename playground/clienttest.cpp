#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res;

  if (getaddrinfo(NULL, "6969", &hints, &res) != 0) {
    spdlog::error("Address not found");
  };

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    spdlog::error("Connection failed");
  };

  char buffer[1024];
  memset(&buffer, 0, 1024);
  strcpy(buffer, "Hello, Server!");

  send(sockfd, buffer, sizeof(buffer), 0);

  read(sockfd, buffer, 1024);
  spdlog::info(buffer);

  freeaddrinfo(res);

  close(sockfd);

  return 0;
}
