#include <fstream>
#include <iostream>
#include <vector>
#include "fun.h"
#include "se-vector.hpp"

bool readLine(std::istream& in, std::string& line) {
  line.clear();
  char ch;
  while (in.get(ch)) {
    if (ch == '\n') {
      return true;
    }
    line.push_back(ch);
  }
  return !line.empty();
}

int main(int argc, char *argv[]) {
  std::istream *input = nullptr;
  std::ifstream fileObj;
  std::ifstream& file = fileObj;

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
    return 1;
  }
  knk::vector< long long > results;
  std::string line;

  while (std::readLine(*input, line)) {
    if (line.empty()) {
      continue;
    }

    ivanov::List< Object * > infixList = stringToInfixList(line);
    ivanov::List< Object * > postfixList;

    try {
      postfixList = infixToPostfix(infixList);
    } catch (const std::exception &e) {
      std::cerr << "Conversion error: " << e.what() << "\n";
      for (Object *obj: infixList) {
        delete obj;
      }
      return 1;
    }

    Integer *result = nullptr;
    try {
      result = eval(postfixList);
      results.pushBack(result->getValue());
      delete result;
    } catch (const std::exception &e) {
      std::cerr << "Evaluation error: " << e.what() << "\n";
      delete result;
      for (Object *obj: postfixList) {
        delete obj;
      }
      return 1;
    }

    for (Object *obj: postfixList) {
      delete obj;
    }
  }
  for (size_t i = results.getSize(); i > 0; --i) {
    std::cout << results[i - 1];
    if (i > 1) {
      std::cout << ' ';
    }
  }
  std::cout << "\n";

  return 0;
}
