#include "../include/client.h"
#include "../include/server.h"
#include <iostream>

int main() {
  Server server = Server("", "6969", 10);
  Client client = Client("", "6969");
  int server_fd = server.create();
  client.create();
  int client_fd = server.accept();
  server.send(client_fd, "Hello, Client!");
  std::string output = client.read(20);
  std::cout << output;
  return 0;
}
