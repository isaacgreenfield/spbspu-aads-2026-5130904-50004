#include "BSTManager.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Bad input error\n";
    return 1;
  }

  ivanov::BSTManager manager;
  manager.loadFromFile(argv[1]);

  std::string line;
  bool isAnything = false;

  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    manager.execute(line, isAnything, false);
  }

  return 0;
}
