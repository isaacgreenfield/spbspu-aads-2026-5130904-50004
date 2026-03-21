#include <stdexcept>
#include <fstream>
#include <cctype>
#include "fun.h"

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;

  std::ifstream(argv[1]);

  return 0;
}