#ifndef LIST_H
#define LIST_H
#include <limits>
#include <stdexcept>

template <class T> class LIter;
template <class T> class LCIter;

template <class T>
class Iter {
public:
  template <class T>
  class List {
    friend class Iter<T>;
    T data;
    List<T>* next;

    Iter<T>* getIter() {
      return new Iter<T>(this);
    }
    List<T>* addFront(T newData) {
      return new List<T>(newData, this);
    }
    void addBack(T newData) {
      next = new List<T>(newData, next);
    }

    List<T>* deleteSelf() {
      List<T>* tmp = next;
      delete this;
      return tmp;
    }

    void copy(List<T>* target) {
      target->data = this->data;
    }

    Iter<T>* getLast() {
      List<T>* tmp = this;
      while (tmp->next != nullptr) {
        tmp = tmp->next;
      }
      return tmp->getIter();
    }
  };

  List<T>* that;

  bool hasNext() {
    return (that != nullptr && that->next != nullptr);
  }
  void next() noexcept{
    that = that->next;
  }

  LIter<T>* getReal() {
    return new LIter<T>(this->that);
  }
  LCIter<T>* getConst() {
    return new LCIter<T>(this->that);
  }
};

template <class T>
class LIter : public Iter<T> {
public:
  using Iter<T>::that;

  void changeData(T newData) {
    that->data = newData;
  }
};

template <class T>
class LCIter : public Iter<T>{
public:
  using Iter<T>::that;

  T getData() {
    return that->data;
  }
};

template <class T>
Iter<T> getLast(Iter<T> head) {
  while (head.that->next != nullptr) head.that = head.that->next;
  return head;
}

template <class T>
void addLast(Iter<T> head, T data) {
  head = getLast(head);
  head.that->next = new Iter<T>::List<T>(data, nullptr);
}

template <class T>
void deleteLast(Iter<T> head) {
  delete getLast(head)->that;
}

template <class T>
void clear(Iter<T> head) {
  while (head.that->next != nullptr) {
    head.that->deleteSelf();
  }
}

template <class T>
Iter<T> copy(Iter<T> thisHead) {
  Iter<T> newHead = *thisHead;
  try {
    Iter<T> tmp = newHead; // i dont think thats correct
    while (thisHead.that->next != nullptr) { //does this work?
      tmp.that->next = new Iter<T>::List<T>(thisHead.that->next->data, nullptr);
      tmp.that = tmp->next;
      thisHead.that = thisHead->next;
    } //do I need to think about last elem?
  } catch (...) { //which error does this throw?
    clear(newHead); //newHead is an object, not sure whether that would be correct
    return nullptr;
  }
  return newHead;
}

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
#endif
