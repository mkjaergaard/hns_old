
#include <hns/server_entity.hpp>

#include <iostream>

int main()
{
  std::cout << "Running" << std::endl;

  hns::ServerEntity server;

  std::cout << "." << std::endl;
  server.registerTag("ns1/ns1_1/t1_1_1");
//  server.registerTag("ns1/ns1_1/t1_1_2");
  std::cout << "." << std::endl;
  server.registerTag("ns1/ns1_2/t1_2_1");
  std::cout << "." << std::endl;
  server.registerTag("ns2/ns2_1/t2_1_1");
  std::cout << "." << std::endl;
  server.registerTag("ns2/ns2_2/t2_2_1");
  std::cout << "." << std::endl;

  server.registerPseudoTag("ns1/t1_1", "ns1_1/t1_1_1");
  std::cout << "." << std::endl;
  server.registerPseudoTag("ns1/t1_1", "ns1_1/t1_1_2");
  std::cout << "." << std::endl;
  server.subscribeTag("ns1/ns1_1/t1_1_1", 0);
  std::cout << "." << std::endl;
  server.subscribeTag("ns1/ns1_1/t1_1_2", 0);
  std::cout << "." << std::endl;

}
