#ifndef LIST_H
#define LIST_H

template <class T> class LIter;
template <class T> class LCIter;

template <class T>
class Iter {
public:
  class List {
  public:
    List(const T& d, List* n = nullptr) : data(d), next(n) {}
    friend class Iter<T>;
    T data;
    List* next;

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

  explicit LIter(typename Iter<T>::List *p) :Iter<T>(p) {}

  void changeData(T newData) {
    that->data = newData;
  }
};

template <class T>
class LCIter : public Iter<T>{
public:
  using Iter<T>::that;

  explicit LCIter(typename Iter<T>::List *p) :Iter<T>(p) {}

  T getData() {
    return that->data;
  }
};
#endif
