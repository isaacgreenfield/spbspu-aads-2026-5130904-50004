#ifndef CITER_HPP
#define CITER_HPP

#include <iterator>
#include "list.hpp"

namespace ivanov {
  template < class T >
  class CIter : public std::iterator<std::forward_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
    friend class List< T >;

  public:
    CIter() noexcept
    : ptr(nullptr) {
    }
    CIter(const CIter&) noexcept = default;
    CIter(CIter&&) noexcept = default;
    ~CIter() = default;
    CIter& operator=(const CIter&) noexcept = default;
    CIter& operator=(CIter&&) noexcept = default;

    const T& operator*() const noexcept {
      return ptr->data;
    }
    const T* operator->() const noexcept {
      return &(ptr->data);
    }

    CIter& operator++() noexcept {
      ptr = ptr->next;
      return *this;
    }

    bool operator==(const CIter& other) const noexcept {
      return ptr == other.ptr;
    }
    bool operator!=(const CIter& other) const noexcept {
      return ptr != other.ptr;
    }

  private:
    const typename List< T >::Elem* ptr;
    explicit CIter(const typename List< T >::Elem* p) noexcept
    : ptr(p) {
    }
  };

  template < class T >
  CIter< T > List< T >::cbegin() const noexcept {
    return CIter< T >(head);
  }
  template < class T >
  CIter< T > List< T >::cend() const noexcept {
    return CIter< T >(nullptr);
  }
}

#endif
