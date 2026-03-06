#include<iostream>
#include "List.h"

using namespace ivanov;

int main(){
  std::string wtmp;
  int ntmp;

  List<std::string> names;
  List<List<int>> nums;

  try {
    while (!std::cin.eof()) {
      std::cin >> wtmp;
      names.push_back(wtmp);
      List<int> curr;
      while (wtmp != "\n") {
        std::cin >> wtmp;
        ntmp = std::stoi(wtmp);
        curr.push_back(ntmp);
      }
      nums.push_back(std::move(curr));
    }
  } catch (std::bad_alloc& e) {
    std::cerr << "Bad allocation faliure\n";
    return 1;
  }

  auto name = names.cbegin();
  for (size_t i = 0; i < names.size(); ++i) {
    std::cout << *name;
    if (i != names.size() - 1) std::cout << " ";
    else std::cout << "\n";
    ++name;
  }



  return 0;
}
