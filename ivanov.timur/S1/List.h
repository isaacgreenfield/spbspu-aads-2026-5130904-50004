#ifndef LIST_H
#define LIST_H
#include <stdexcept>

template <class T>
class List {
  T data;
  List<T>* next = nullptr;
};

template <class T> class LCIter;

template <class T>
class Iter {
public:
  friend class List<T>;
  List<T>* that;

private:
  bool isNext() {
    return that->next != nullptr;
  }
  void next() {
    if (isNext()) that = that->next;
    else {
      delete that;
      throw std::logic_error("Cannot get next if there is no next :(");
    }
  }
};

template <class T>
class LIter : private Iter<T> {
  using Iter<T>::that;

  void changeData(T newData) {
    that->data = newData;
  }
  LCIter<T> throwConst() {
    return new LCIter<T>(that);
  }
};

template <class T>
class LCIter : private Iter<T>{
  using Iter<T>::that;

  T getData() {
    return that->data;
  }
  LIter<T> throwConst() {
    return new LIter<T>(that);
  }
};

template <class T>
LCIter<T> getFirstIter(List<T>* head) {
  return new LCIter<T>(head);
}

template <class T>
LCIter<T> getLast(List<T>* head) {
  List<T>* tmp = head;
  while (tmp->next != nullptr) tmp = tmp->next;
  return new LCIter<T>(tmp);
}

template <class T>
List<T>* addHead(List<T>* head, T data) {
  List<T> tmp = new List<T>(data, head);
  return tmp;
}

template <class T>
void addLast(List<T>* head, T data) {
  while (head->next != nullptr) head = head->next;
  head->next = new List<T>*(data);
}

template <class T>
List<T>* deleteHead(List<T>* head) {
  List<T>* tmp = head->next;
  delete head;
  return tmp;
}

template <class T>
void deleteLast(List<T>* head) {
  while (head->next != nullptr) head = head->next;
  delete head;
}

template <class T>
void clear(List<T>* head) {
  while (head->next != nullptr) head = deleteHead(head);
  delete head;
}

template <class T>
List<T>* copy(List<T>* thisHead) {
  List<T> newHead = *thisHead;
  try {
    List<T>* tmp = newHead;
    while (thisHead != nullptr) {
      tmp->next = new List<T>(thisHead->next->data, nullptr);
      tmp = tmp->next;
      thisHead = thisHead->next;
    }
  } catch (...) {
    clear(newHead);
    return nullptr;
  }
  return newHead;
}

template <class T>
List<T>* move(List<T>* thisHead) {
  List<T>* newHead = new List<T>(thisHead->data, thisHead->next);
  try {
    List<T>* tmp = deleteHead(thisHead);
  } catch (...) {
    delete newHead;
    return nullptr;
  }
  return newHead;
}
#endif
