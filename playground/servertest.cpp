#include <cstring>
#include <netdb.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(NULL, "6969", &hints, &res) != 0) {
    spdlog::error("Address not found");
    return 1;
  };

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (sockfd == -1) {
    spdlog::error("Could not create socket");
    return 1;
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    spdlog::error("Could not bind address to socket");
    return 1;
  };

  freeaddrinfo(res);

  if (listen(sockfd, 10) != 0) {
    spdlog::error("Could not start listening for connctions");
    return 1;
  };

  spdlog::info("Server listening");

  char buffer[2048];

  while (true) {
    int fd = accept(sockfd, NULL, NULL);
    if (fd == -1) {
      spdlog::error("Could not accept connection");
      return 1;
    }
    int code = read(fd, buffer, 2048);
    spdlog::info(buffer);
    memset(buffer, 0, 2048);
    write(fd, "Hello, Client", 13);
    if (code == 0) {
      spdlog::warn("Connection closed");
    } else if (code == -1) {
      spdlog::error("Failed to read fd");
      return 1;
    }
    close(fd);
  }

  return 0;
}
