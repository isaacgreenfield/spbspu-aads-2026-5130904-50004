#include <iostream>
#include <string>
#include "Graph.h"

int main(int argc, char* argv[]) {
  bool isAnything  = false;
  if (argc != 2) {
    std::cerr << "Wrong" << std::endl;
    std::cout << "\n";
    return 1;
  }

  ivanov::GraphManager manager;
  manager.loadFromFile(argv[1]);
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    manager.execute(line, isAnything, false);
  }
  if(!isAnything) std::cout << "\n";
  return 0;
}
