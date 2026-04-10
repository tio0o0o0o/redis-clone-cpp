#include "../include/client.h"
#include "../include/server.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  std::unique_ptr<Server> server_ptr = nullptr;
  int client_fd = -1;
  int server_fd = -1;

  while (true) {
    std::cout << "Enter command (create, accept, send, read): ";
    std::string command;
    std::cin >> command;

    if (command == "create") {
      server_ptr = std::make_unique<Server>("", "6969", 10);
      server_fd = server_ptr->create();
    } else if (command == "accept") {
      client_fd = server_ptr->accept();
    } else if (command == "send") {
      std::cout << "Enter message: ";
      std::string message;
      std::cin >> message;
      server_ptr->send(client_fd, message);
    } else if (command == "read") {
      std::cout << '\n' << server_ptr->read(client_fd, 1024);
    }
  }

  return 0;
}
