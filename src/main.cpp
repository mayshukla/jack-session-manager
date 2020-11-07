#include <string>
#include <vector>

#include "JackClient.h"

int main() {
  JackClient client("jack-session-manager");

  std::vector<std::string> ports = client.getPorts();
  for (std::string &port : ports) {
    std::cout << port << std::endl;
  }

  return 0;
}
