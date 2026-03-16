#ifndef STACK_H
#define STACK_H
#include "List.h"

template <typename T>
class Stack {
  ivanov::List<T>* head;
public:
  Stack(ivanov::List<T>* nwh) noexcept;
  ~Stack() = default;
  void push(T rhs);
  T drop();
  bool isEmpty();
};

template<typename T>
bool Stack<T>::isEmpty() {
  return head->empty();
}

template<typename T>
T Stack<T>::drop() {
  if (!isEmpty()) {
    T ans = head->back();
    head->pop_back();
    return ans;
  }
  throw std::logic_error("no elems");
}

template<typename T>
Stack<T>::Stack(ivanov::List<T>* nwh): head(nwh) {}

template<typename T>
void Stack<T>::push(T rhs) {
  if (!isEmpty) {
    head->push_back(rhs);
  }
  else {
    head->push_back();
    head->pop_front();
  }
}

#endif //STACK_H
