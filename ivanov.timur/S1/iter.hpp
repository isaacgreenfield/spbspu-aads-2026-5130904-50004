#ifndef ITER_HPP
#define ITER_HPP

#include <iterator>

#include "list.hpp"

namespace ivanov
{
  template< class T >
  class Iter : public std::iterator< std::forward_iterator_tag, T >
  {
    friend class List< T >;

  public:
    Iter() noexcept
      : ptr_(nullptr)
    {
    }

    Iter(const Iter&) noexcept = default;
    Iter(Iter&&) noexcept = default;
    ~Iter() = default;

    Iter& operator=(const Iter&) noexcept = default;
    Iter& operator=(Iter&&) noexcept = default;

    T& operator*() const noexcept
    {
      return ptr_->data_;
    }

    T* operator->() const noexcept
    {
      return &(ptr_->data_);
    }

    Iter& operator++() noexcept
    {
      ptr_ = ptr_->next_;
      return *this;
    }

    bool operator==(const Iter& other) const noexcept
    {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const Iter& other) const noexcept
    {
      return ptr_ != other.ptr_;
    }

  private:
    typename List< T >::Elem* ptr_;

    explicit Iter(typename List< T >::Elem* p) noexcept
      : ptr_(p)
    {
    }
  };

  template< class T >
  Iter< T > List< T >::begin() const noexcept
  {
    return Iter< T >(head_);
  }

  template< class T >
  Iter< T > List< T >::end() const noexcept
  {
    return Iter< T >(nullptr);
  }

  template< class T >
  Iter< T > List< T >::insert_after(const Iter< T > pos, const T& value)
  {
    Elem* curr = pos.ptr_;
    if (curr == nullptr) {
      throw std::out_of_range("cannot insert in nullptr");
    }
    Elem* nw = new Elem(value, curr->next_);
    curr->next_ = nw;
    if (curr == tail_) {
      tail_ = nw;
    }
    sz_++;
    return Iter< T >(nw);
  }

  template< class T >
  Iter< T > List< T >::insert_after(const Iter< T > pos, T&& value)
  {
    Elem* curr = pos.ptr_;
    if (curr == nullptr) {
      throw std::out_of_range("cannot insert in nullptr");
    }
    Elem* nw = new Elem(std::move(value), curr->next_);
    curr->next_ = nw;
    if (curr == tail_) {
      tail_ = nw;
    }
    sz_++;
    return Iter< T >(nw);
  }

  template< class T >
  Iter< T > List< T >::erase_after(const Iter< T > pos)
  {
    Elem* curr = pos.ptr_;
    if (curr == nullptr || curr->next_ == nullptr) {
      throw std::out_of_range("cannot erase from nullptr");
    }
    Elem* tmp = curr->next_->next_;
    if (curr->next_ == tail_) {
      tail_ = curr;
    }
    delete curr->next_;
    curr->next_ = tmp;
    sz_--;
    return Iter< T >(curr->next_);
  }
}

#endif
