#include "../include/client.h"
#include "../include/server.h"
#include <iostream>
#include <memory>

int main() {
  std::unique_ptr<Client> client_ptr = nullptr;
  int client_fd = -1;

  while (true) {
    std::cout << "Enter command (create, send, read): ";
    std::string command;
    std::cin >> command;

    if (command == "create") {
      client_ptr = std::make_unique<Client>("", "6969");
      client_fd = client_ptr->create();
    } else if (command == "send") {
      std::cout << "Enter message: ";
      std::string message;
      std::cin >> message;
      client_ptr->send(message);
    } else if (command == "read") {
      std::cout << '\n' << client_ptr->read(1024);
    }
  }

  return 0;
}
