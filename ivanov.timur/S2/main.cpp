#include <fstream>
#include <iostream>
#include "fun.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Not enough arguments\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "No file provided\n";
  }

  std::string line;
  while (std::getline(file, line)) {
    ivanov::List<Object*> infixList = stringToInfixList(line);
    ivanov::List<Object*> postfixList;

    try {
      postfixList = infixToPostfix(infixList);
    } catch (...) {
      std::cerr << "Conversion error\n";
      for (Object* obj : infixList) delete obj;
      return 1;
    }

    for (auto it = postfixList.begin(); it != postfixList.end(); ++it) {
      Object* obj = *it;
      if (obj->isNumber()) {
        Integer* num = static_cast<Integer*>(obj);
        std::cout << num->getValue();
      } else {
        std::cout << obj->symbol;
      }
      auto next = it;
      ++next;
      if (next != postfixList.end()) std::cout << ' ';
    }

    try {
      Integer* result = eval(postfixList);
      std::cout << "Result: " << result->getValue() << '\n';
      delete result;
    } catch (...) {
      std::cerr << "Evaluation error\n";
    }

    for (Object* obj : infixList) delete obj;
    for (Object* obj : postfixList) delete obj;
  }

  return 0;
}