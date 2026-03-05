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
  return 0;
}
