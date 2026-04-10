#pragma once
#include "./socket.h"
#include <cstddef>
#include <string>

class Client : public Socket {
public:
  Client(std::string ip, std::string port) : Socket(ip, port) {}

  int create();

  int send(std::string buffer);

  std::string read(size_t size);
};
