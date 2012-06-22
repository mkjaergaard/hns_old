
#include <hns/server_entity.hpp>

#include <iostream>

int main()
{
  std::cout << "Running" << std::endl;

  hns::ServerEntity server;

  server.registerTag("my/new/namespace");
  server.registerTag("my/new/namespace");

  server.subscribeTag("my/new/namespace", 0);

}
