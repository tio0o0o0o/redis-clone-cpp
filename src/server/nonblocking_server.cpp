#include "../../include/dispatch.h"
#include "../../include/resp2.h"
#include "../../include/server.h"
#include "../../include/store.h"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
  Store store;
  Server server("127.0.0.1", "6969", 100);

  int epoll_fd = epoll_create1(0);

  epoll_event ready_fds[64];

  int server_fd = server.create();

  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = server_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

  while (true) {
    int n = epoll_wait(epoll_fd, ready_fds, 64, -1);
    for (int i = 0; i < n; i++) {
      if (ready_fds[i].data.fd == server_fd) {
        int new_client_fd = server.accept();
        epoll_event new_ev;
        new_ev.events = EPOLLIN;
        new_ev.data.fd = new_client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client_fd, &new_ev);
      } else {
        std::string command_encoded = server.read(ready_fds[i].data.fd, 2048);
        if (command_encoded.empty()) {
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ready_fds[i].data.fd, nullptr);
          close(ready_fds[i].data.fd);
          continue;
        }
        auto commands_parsed =
            std::get<std::vector<std::shared_ptr<RespValue>>>(
                parse(command_encoded).resp_value.value);
        std::vector<std::string> commands;
        for (int i = 0; i < commands_parsed.size(); i++) {
          commands.push_back(std::get<std::string>(commands_parsed[i]->value));
        }
        RespValue resp_value = execute(commands, store);
        std::string return_encoded = serialize(resp_value);
        int send_res = server.send(ready_fds[i].data.fd, return_encoded);
      }
    }
  }
}
