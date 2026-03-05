#ifndef LIST_H
#define LIST_H
#include <stdexcept>

template <class T> class LIter;
template <class T> class LCIter;

template <class T>
class Iter {
public:
  class List {
  public:
    friend class Iter<T>;
    T data;
    List* next;

    List(const T& d, List* n = nullptr) : data(d), next(n) {}
    Iter<T> getIter() {
      return Iter<T>(this);
    }
    List* addFront(T newData) {
      return new List(newData, this);
    }
    void addBack(T newData) {
      next = new List(newData, next);
    }

    List* deleteSelf() {
      List* tmp = next;
      delete this;
      return tmp;
    }

    void copy(List* target) {
      target->data = this->data;
    }

    Iter<T> getLast() {
      List* tmp = this;
      while (tmp->next != nullptr) {
        tmp = tmp->next;
      }
      return tmp->getIter();
    }
  };

  List* that;

  explicit Iter(List* p) : that(p) {}

  bool hasNext() {
    return (that != nullptr && that->next != nullptr);
  }
  void next() noexcept{
    that = that->next;
  }

  LIter<T> getReal() {
    return LIter<T>(this->that);
  }
  LCIter<T> getConst() {
    return LCIter<T>(this->that);
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
Iter<T> getLast(Iter<T> head) noexcept{
  while (head.that->next != nullptr) head.that = head.that->next;
  return head;
}

template <class T>
Iter<T> getPrevLast(Iter<T> head) {
  if (head.that == nullptr || head.that->next == nullptr) throw std::logic_error("cannot find prev last");
  while (head.that->next->next != nullptr) head.that = head.that->next;
  return head;
}

template <class T>
void addLast(Iter<T> head, T data) {
  if (head.that == nullptr) head = new Iter<T>::List(data, nullptr);
  else {
    head = getLast(head);
    head.that->next = new Iter<T>::List(data, nullptr);
  }
}

template <class T>
void deleteLast(Iter<T> head) noexcept{
  delete getPrevLast(head).that->next;
  getPrevLast(head).that->next = nullptr;
}

template <class T>
void clear(Iter<T> head) noexcept{
  if (head.that != nullptr) {
    while (head.that->next != nullptr) {
      head.that->deleteSelf();
    }
  }
  delete head.that;
}

template <class T>
Iter<T> copy(Iter<T> thisHead) {
  Iter<T> newHead = Iter<T>(thisHead.that);
  if (thisHead.that == nullptr) throw std::logic_error("cannot use nullptr state");
  try {
    Iter<T> tmp = newHead; // i dont think thats correct
    while (thisHead.that->next != nullptr) { //does this work?
      tmp.that->next = new Iter<T>::List(thisHead.that->next->data, nullptr);
      tmp.that = tmp.that->next;
      thisHead.that = thisHead.that->next;
    } //do I need to think about last elem?
  } catch (...) { //which error does this throw?
    clear(newHead); //newHead is an object, not sure whether that would be correct
    throw std::logic_error("copy faliure fatal");
  }
  return newHead;
}
#endif
