#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>
#include <limits>

namespace ivanov {
  template <class T> class List;

  template <class T>
  class Iter {
    friend class List<T>;

  public:
    Iter()  : ptr(nullptr) {}
    Iter(const Iter&)  = default;
    Iter(Iter&&)  = default;
    ~Iter() = default;
    Iter& operator=(const Iter&)  = default;
    Iter& operator=(Iter&&)  = default;

    T& operator*() const  {
      return ptr->data;
    }
    T* operator->() const  {
      return &(ptr->data);
    }

    Iter& operator++()  {
      ptr = ptr->next;
      return *this;
    }

    bool operator==(const Iter& other) const  {
      return ptr == other.ptr;
    }
    bool operator!=(const Iter& other) const  {
      return ptr != other.ptr;
    }

  private:
    typename List<T>::Elem* ptr;
    explicit Iter(typename List<T>::Elem* p)  : ptr(p) {}
  };

  template <class T>
  class CIter {
    friend class List<T>;

  public:
    CIter()  : ptr(nullptr) {}
    CIter(const CIter&)  = default;
    CIter(CIter&&)  = default;
    ~CIter() = default;
    CIter& operator=(const CIter&)  = default;
    CIter& operator=(CIter&&)  = default;

    const T& operator*() const  {
      return ptr->data;
    }
    const T* operator->() const  {
      return &(ptr->data);
    }

    CIter& operator++()  {
      ptr = ptr->next;
      return *this;
    }

    bool operator==(const CIter& other) const  {
      return ptr == other.ptr;
    }
    bool operator!=(const CIter& other) const  {
      return ptr != other.ptr;
    }

  private:
    const typename List<T>::Elem* ptr;
    explicit CIter(const typename List<T>::Elem* p)  : ptr(p) {}
  };

  template <class T>
  class List {
    friend class Iter<T>;
    friend class CIter<T>;
  protected:
    struct Elem {
      T data;
      Elem* next;
      explicit Elem(const T& val, Elem* nxt = nullptr) : data(val), next(nxt) {}
      explicit Elem(T&& val, Elem* nxt = nullptr) : data(std::move(val)), next(nxt) {}
    };

    Elem* head;
    Elem* tail;
    size_t sz;

  public:
    List()  : head(nullptr), tail(nullptr), sz(0) {}

    ~List() {
      clear();
    }

    List(const List& other) : head(nullptr), tail(nullptr), sz(0) {
      for (Elem* curr = other.head; curr != nullptr; curr = curr->next) {
        push_back(curr->data);
      }
    }

    List(List&& other)  : head(other.head), tail(other.tail), sz(other.sz) {
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

    List& operator=(List&& other)  {
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

    void swap(List& other)  {
      Elem *tmp_head = head;
      Elem *tmp_tail = tail;
      size_t tmp_sz = sz;
      head = other.head;
      tail = other.tail;
      sz = other.sz;
      other.head = tmp_head;
      other.tail = tmp_tail;
      other.sz = tmp_sz;
    }

    Iter<T> begin() const  {
      return Iter<T>(head);
    };
    Iter<T> end() const  {
      return Iter<T>(nullptr);
    };

    CIter<T> cbegin() const  {
      return CIter<T>(head);
    }
    CIter<T> cend() const  {
      return CIter<T>(nullptr);
    }

    bool empty() const  {
      return sz == 0;
    };
    size_t size() const  {
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
      if (empty()) tail = nw;
      head = nw;
      sz++;
    };
    void push_front(T&& value) {
      Elem* nw = new Elem(std::move(value), head);
      if (empty()) tail = nw;
      head = nw;
      sz++;
    };
    void pop_front() {
      if (empty()) return;
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
      if (empty()) return;
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

    Iter<T> insert_after(Iter<T> pos, const T& value) {
      Elem* curr = pos.ptr;
      if (curr == nullptr)
        throw std::out_of_range("cannot insert in nullptr");
      Elem* nw = new Elem(value, curr->next);
      curr->next = nw;
      if (curr == tail) {
        tail = nw;
      }
      sz++;
      return Iter<T>(nw);
    };
    Iter<T> insert_after(Iter<T> pos, T&& value) {
      Elem* curr = pos.ptr;
      if (curr == nullptr)
        throw std::out_of_range("cannot insert in nullptr");
      Elem* nw = new Elem(std::move(value), curr->next);
      curr->next = nw;
      if (curr == tail) {
        tail = nw;
      }
      sz++;
      return Iter<T>(nw);
    };

    Iter<T> erase_after(Iter<T> pos) {
      Elem* curr = pos.ptr;
      if (curr == nullptr || curr->next == nullptr)
        throw std::out_of_range("cannot erase from nullptr");
      Elem* tmp = curr->next->next;
      if (curr->next == tail) tail = curr;
      delete curr->next;
      curr->next = tmp;
      sz--;
      return Iter<T>(curr->next);
    };

    void clear() {
      while (head != nullptr) pop_front();
    };
  };

  template <class T>
  inline void sum(T& a, const T& b) {
    if (std::numeric_limits<T>::max() - b < a) {
      throw std::overflow_error("Overflow");
    }
    a += b;
  }
}

#endif
