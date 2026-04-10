#pragma once
#include "./socket.h"
#include <optional>
#include <string>

class Server : public Socket {
public:
  Server(std::string ip, std::string port, int backlog)
      : Socket(ip, port), backlog(backlog) {}

  int create();

  int accept();

private:
  int backlog;
};
