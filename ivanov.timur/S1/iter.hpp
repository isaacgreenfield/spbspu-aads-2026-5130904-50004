#ifndef ITER_HPP
#define ITER_HPP

#include <iterator>
#include "List.h"

namespace ivanov {
  template < class T >
  class Iter : public std::iterator<std::forward_iterator_tag, T> {
    friend class List< T >;

  public:
    Iter() noexcept
    : ptr(nullptr) {
    }
    Iter(const Iter&) noexcept = default;
    Iter(Iter&&) noexcept = default;
    ~Iter() = default;
    Iter& operator=(const Iter&) noexcept = default;
    Iter& operator=(Iter&&) noexcept = default;

    T& operator*() const noexcept {
      return ptr->data;
    }
    T* operator->() const noexcept {
      return &(ptr->data);
    }

    Iter& operator++() noexcept {
      ptr = ptr->next;
      return *this;
    }

    bool operator==(const Iter& other) const noexcept {
      return ptr == other.ptr;
    }
    bool operator!=(const Iter& other) const noexcept {
      return ptr != other.ptr;
    }

  private:
    typename List< T >::Elem* ptr;
    explicit Iter(typename List< T >::Elem* p) noexcept
    : ptr(p) {
    }
  };

  template < class T >
  Iter< T > List< T >::begin() const noexcept {
    return Iter< T >(head);
  }
  template < class T >
  Iter< T > List< T >::end() const noexcept {
    return Iter< T >(nullptr);
  }

  template < class T >
  Iter< T > List< T >::insert_after(Iter< T > pos, const T& value) {
    Elem* curr = pos.ptr;
    if (curr == nullptr) {
      throw std::out_of_range("cannot insert in nullptr");
    }
    Elem* nw = new Elem(value, curr->next);
    curr->next = nw;
    if (curr == tail) {
      tail = nw;
    }
    sz++;
    return Iter< T >(nw);
  }
  template < class T >
  Iter< T > List< T >::insert_after(Iter< T > pos, T&& value) {
    Elem* curr = pos.ptr;
    if (curr == nullptr) {
      throw std::out_of_range("cannot insert in nullptr");
    }
    Elem* nw = new Elem(std::move(value), curr->next);
    curr->next = nw;
    if (curr == tail) {
      tail = nw;
    }
    sz++;
    return Iter< T >(nw);
  }
  template < class T >
  Iter< T > List< T >::erase_after(Iter< T > pos) {
    Elem* curr = pos.ptr;
    if (curr == nullptr || curr->next == nullptr) {
      throw std::out_of_range("cannot erase from nullptr");
    }
    Elem* tmp = curr->next->next;
    if (curr->next == tail) tail = curr;
    delete curr->next;
    curr->next = tmp;
    sz--;
    return Iter< T >(curr->next);
  }
}

#endif
