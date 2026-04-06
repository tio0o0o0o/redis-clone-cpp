#include "../include/logger.h"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(NULL, "6969", &hints, &res) != 0) {
    LOG_ERROR("Address not found");
    return 1;
  };

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (sockfd == -1) {
    LOG_ERROR("Could not create socket");
    return 1;
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    LOG_ERROR("Could not bind address to socket");
    return 1;
  };

  freeaddrinfo(res);

  if (listen(sockfd, 10) != 0) {
    LOG_ERROR("Could not start listening for connctions");
    return 1;
  };

  LOG_INFO("Server listening");

  char buffer[2048];

  while (true) {
    int fd = accept(sockfd, NULL, NULL);
    if (fd == -1) {
      LOG_ERROR("Could not accept connection");
      return 1;
    }
    int code = read(fd, buffer, 2048);
    LOG_INFO(buffer);
    memset(buffer, 0, 2048);
    write(fd, "Hello, Client", 13);
    if (code == 0) {
      LOG_INFO("Connection closed");
    } else if (code == -1) {
      LOG_ERROR("Failed to read fd");
      return 1;
    }
    close(fd);
  }

  return 0;
}
