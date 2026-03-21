#ifndef QUEUE_H
#define QUEUE_H
#include "List.h"
namespace ivanov {
  template <typename T>
  class Queue {
    ivanov::List<T>* head;
  public:
    Queue(ivanov::List<T>* nwh);
    ~Queue() = default;
    void push (T rhs);
    T drop();
    bool isEmpty();
  };

  template<typename T>
  bool Queue<T>::isEmpty() {
    return head->empty();
  }

  template<typename T>
  Queue<T>::Queue(ivanov::List<T> *nwh): head(nwh) {}

  template<typename T>
  T Queue<T>::drop() {
    if (!isEmpty()) {
      T ans = head->front();
      head->pop_front();
      return ans;
    }
    throw std::logic_error("no elems");
  }

  template<typename T>
  void Queue<T>::push(T rhs) {
    if (!isEmpty) head->push_back(rhs);
    else {
      head = new ivanov::List<T>();
      head->push_back(rhs);
      head->pop_front();
    }
  }
}
#endif //QUEUE_H
