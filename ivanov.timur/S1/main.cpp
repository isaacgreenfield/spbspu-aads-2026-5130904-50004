#include<iostream>
#include "List.h"
#include <limits>

template <class T>
size_t sum(LCIter<T> target) {
  size_t ans = target.getData();
  while (target.hasNext()) {
    target.next();
    if (std::numeric_limits<size_t>::max() - ans < target.getData()) {
      throw std::logic_error("Target exceeds numeric limits");
    }
    ans += target.getData();
  }
  return ans;
}

int main(){
  Iter<std::string> names = Iter<std::string>(new Iter<std::string>::List("0", nullptr));
  Iter<Iter<size_t>> numbers = Iter<Iter<size_t>>(new Iter<Iter<size_t>>::List(Iter<size_t>(new Iter<size_t>::List(0, nullptr)), nullptr));

  std::string tmp;
  while (!std::cin.eof()) {
    std::cin >> tmp;
    try {
      int t = std::stoi(tmp);
      size_t tm = static_cast<size_t>(t);
      numbers.that->data.that->addBack(tm);
    } catch (...) {
      names.that->addBack(tmp);
      numbers.that->addBack(Iter<size_t>(new Iter<size_t>::List(0, nullptr)));
    }
  }

  while ()

  return 0;
}
