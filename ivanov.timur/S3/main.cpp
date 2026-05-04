#include <iostream>
#include <string>
#include "Graph.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong" << std::endl;
    return 1;
  }

  ivanov::GraphManager manager;
  manager.loadFromFile(argv[1]);
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    manager.execute(line, false);
  }
  return 0;
}
