#include <fstream>
#include <iostream>
#include "fun.h"

int main(int argc, char* argv[]) {
  std::istream* input = nullptr;
  std::ifstream file;

  if (argc == 1) {
    input = &std::cin;
  } else if (argc == 2) {
    file.open(argv[1]);
    if (!file.is_open()) {
      std::cerr << "Cannot open file: " << argv[1] << "\n";
      return 1;
    }
    input = &file;
  } else {
    std::cerr << "Usage: " << argv[0] << " [input_file]\n";
    std::cerr << "   no arguments : read from stdin\n";
    std::cerr << "   one argument : read from file\n";
    return 1;
  }

  std::string line;
  while (std::getline(*input, line)) {
    ivanov::List<Object*> infixList = stringToInfixList(line);
    ivanov::List<Object*> postfixList;

    try {
      postfixList = infixToPostfix(infixList);
    } catch (...) {
      std::cerr << "Conversion error\n";
      for (Object* obj : infixList) delete obj;
      return 1;
    }

    try {
      Integer* result = eval(postfixList);
      std::cout << result->getValue() << '\n';
      delete result;
    } catch (...) {
      std::cerr << "Evaluation error\n";
    }

    for (Object* obj : postfixList) delete obj;
    std::cout << "\n";
  }

  return 0;
}
