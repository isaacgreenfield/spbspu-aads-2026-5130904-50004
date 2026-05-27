#ifndef CITER_HPP
#define CITER_HPP

#include <iterator>

#include "list.hpp"

namespace ivanov
{
  template< class T >
  class CIter : public std::iterator< std::forward_iterator_tag, T, std::ptrdiff_t, const T*, const T& >
  {
    friend class List< T >;

  public:
    CIter() noexcept
      : ptr_(nullptr)
    {
    }

    CIter(const CIter&) noexcept = default;
    CIter(CIter&&) noexcept = default;
    ~CIter() = default;

    CIter& operator=(const CIter&) noexcept = default;
    CIter& operator=(CIter&&) noexcept = default;

    const T& operator*() const noexcept
    {
      return ptr_->data_;
    }

    const T* operator->() const noexcept
    {
      return &(ptr_->data_);
    }

    CIter& operator++() noexcept
    {
      ptr_ = ptr_->next_;
      return *this;
    }

    bool operator==(const CIter& other) const noexcept
    {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const CIter& other) const noexcept
    {
      return ptr_ != other.ptr_;
    }

  private:
    const typename List< T >::Elem* ptr_;

    explicit CIter(const typename List< T >::Elem* p) noexcept
      : ptr_(p)
    {
    }
  };

  template< class T >
  CIter< T > List< T >::cbegin() const noexcept
  {
    return CIter< T >(head_);
  }

  template< class T >
  CIter< T > List< T >::cend() const noexcept
  {
    return CIter< T >(nullptr);
  }
}

#endif
