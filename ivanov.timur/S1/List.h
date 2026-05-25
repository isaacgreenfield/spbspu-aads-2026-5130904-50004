#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>
#include <limits>

namespace ivanov {
  template < class T >
  class Iter;
  template < class T >
  class CIter;

  template < class T >
  class List {
    friend class Iter< T >;
    friend class CIter< T >;
  protected:
    struct Elem {
      T data;
      Elem* next;
      explicit Elem(const T& val, Elem* nxt = nullptr)
      : data(val), next(nxt) {
      }
      explicit Elem(T&& val, Elem* nxt = nullptr)
      : data(std::move(val)), next(nxt) {
      }
    };

    Elem* head;
    Elem* tail;
    size_t sz;

  public:
    List() noexcept
    : head(nullptr), tail(nullptr), sz(0) {
    }

    ~List() {
      clear();
    }

    List(const List& other)
    : head(nullptr), tail(nullptr), sz(0) {
      for (Elem* curr = other.head; curr != nullptr; curr = curr->next) {
        push_back(curr->data);
      }
    }

    List(List&& other) noexcept : head(other.head), tail(other.tail), sz(other.sz) {
      other.head = nullptr;
      other.tail = nullptr;
      other.sz = 0;
    }

    List& operator=(const List& other) {
      if (this != &other) {
        List tmp(other);
        swap(tmp);
      }
      return *this;
    }

    List& operator=(List&& other) noexcept {
      if (this != &other) {
        clear();
        head = other.head;
        tail = other.tail;
        sz   = other.sz;
        other.head = nullptr;
        other.tail = nullptr;
        other.sz = 0;
      }
      return *this;
    }

    void swap(List& other) noexcept {
      std::swap(head, other.head);
      std::swap(tail, other.tail);
      std::swap(sz, other.sz);
    }

    Iter< T > begin() const noexcept;
    Iter< T > end() const noexcept;
    CIter< T > cbegin() const noexcept;
    CIter< T > cend() const noexcept;

    bool empty() const noexcept {
      return sz == 0;
    };
    size_t size() const noexcept {
      return sz;
    };

    T& front() {
      return head->data;
    };
    const T& front() const {
      return head->data;
    };
    T& back() {
      return tail->data;
    };
    const T& back() const {
      return tail->data;
    };

    void push_front(const T& value) {
      Elem* nw = new Elem(value, head);
      if (empty()) {
        tail = nw;
      }
      head = nw;
      sz++;
    };
    void push_front(T&& value) {
      Elem* nw = new Elem(std::move(value), head);
      if (empty()) {
        tail = nw;
      }
      head = nw;
      sz++;
    };
    void pop_front() {
      if (empty()) {
        return;
      }
      if (head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
      } else {
        Elem* tmp = head->next;
        delete head;
        head = tmp;
      }
      sz--;
    };

    void push_back(const T& value) {
      if (empty()) {
        head = new Elem(value);
        tail = head;
      } else {
        tail->next = new Elem(value);
        tail = tail->next;
      }
      sz++;
    };
    void push_back(T&& value) {
      if (empty()) {
        head = new Elem(std::move(value));
        tail = head;
      } else {
        tail->next = new Elem(std::move(value));
        tail = tail->next;
      }
      sz++;
    };
    void pop_back() {
      if (empty()) {
        return;
      }
      if (head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
      } else {
        Elem* tmp = head;
        while (tmp->next != tail) tmp = tmp->next;
        tmp->next = nullptr;
        delete tail;
        tail = tmp;
      }
      sz--;
    };

    Iter< T > insert_after(Iter< T > pos, const T& value);
    Iter< T > insert_after(Iter< T > pos, T&& value);
    Iter< T > erase_after(Iter< T > pos);

    void clear() {
      while (head != nullptr) pop_front();
    };
  };

  template < class T >
  inline void sum(T& a, const T& b) {
    if (std::numeric_limits<T>::max() - b < a) {
      throw std::overflow_error("Overflow");
    }
    a += b;
  }
}

#include "iter.hpp"
#include "citer.hpp"

#endif
