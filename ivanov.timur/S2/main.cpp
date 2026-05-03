#include <fstream>
#include <iostream>
#include <vector>
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
    return 1;
  }
  std::vector<long long> results;
  std::string line;

  while (std::getline(*input, line)) {
    if (line.empty()) continue;

    ivanov::List<Object*> infixList = stringToInfixList(line);
    ivanov::List<Object*> postfixList;

    try {
      postfixList = infixToPostfix(infixList);
    } catch (const std::exception& e) {
      std::cerr << "Conversion error: " << e.what() << "\n";
      for (Object* obj : infixList) delete obj;
      return 1;
    }

    Integer* result = nullptr;
    try {
      result = eval(postfixList);
      results.push_back(result->getValue());
      delete result;
    } catch (const std::exception& e) {
      std::cerr << "Evaluation error: " << e.what() << "\n";
      delete result;
      for (Object* obj : postfixList) delete obj;
      return 1;
    }

    for (Object* obj : postfixList) delete obj;
  }
  for (auto it = results.rbegin(); it != results.rend(); ++it) {
    std::cout << *it;
    if (std::next(it) != results.rend()) std::cout << ' ';
  }
  std::cout << std::endl;

  return 0;
}
