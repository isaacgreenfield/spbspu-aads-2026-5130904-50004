#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>
#include <limits>

namespace ivanov
{
  template < class T >
  class Iter;
  template < class T >
  class CIter;

  template < class T >
  class List
  {
    friend class Iter< T >;
    friend class CIter< T >;

  protected:
    struct Elem
    {
      T data_;
      Elem* next_;

      explicit Elem(const T& val, Elem* nxt = nullptr)
        : data_(val),
          next_(nxt)
      {
      }

      explicit Elem(T&& val, Elem* nxt = nullptr)
        : data_(std::move(val)),
          next_(nxt)
      {
      }
    };

    Elem* head_;
    Elem* tail_;
    size_t sz_;

  public:
    List() noexcept
      : head_(nullptr),
        tail_(nullptr),
        sz_(0)
    {
    }

    ~List()
    {
      clear();
    }

    List(const List& other)
      : head_(nullptr),
        tail_(nullptr),
        sz_(0)
    {
      for (Elem* curr = other.head_; curr != nullptr; curr = curr->next_) {
        push_back(curr->data_);
      }
    }

    List(List&& other) noexcept
      : head_(other.head_),
        tail_(other.tail_),
        sz_(other.sz_)
    {
      other.head_ = nullptr;
      other.tail_ = nullptr;
      other.sz_ = 0;
    }

    List& operator=(const List& other)
    {
      if (this != &other) {
        List tmp(other);
        swap(tmp);
      }
      return *this;
    }

    List& operator=(List&& other) noexcept
    {
      if (this != &other) {
        clear();
        head_ = other.head_;
        tail_ = other.tail_;
        sz_ = other.sz_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.sz_ = 0;
      }
      return *this;
    }

    void swap(List& other) noexcept
    {
      std::swap(head_, other.head_);
      std::swap(tail_, other.tail_);
      std::swap(sz_, other.sz_);
    }

    Iter< T > begin() const noexcept;
    Iter< T > end() const noexcept;
    CIter< T > cbegin() const noexcept;
    CIter< T > cend() const noexcept;

    bool empty() const noexcept
    {
      return sz_ == 0;
    }

    size_t size() const noexcept
    {
      return sz_;
    }

    T& front()
    {
      return head_->data_;
    }

    const T& front() const
    {
      return head_->data_;
    }

    T& back()
    {
      return tail_->data_;
    }

    const T& back() const
    {
      return tail_->data_;
    }

    void push_front(const T& value)
    {
      Elem* nw = new Elem(value, head_);
      if (empty()) {
        tail_ = nw;
      }
      head_ = nw;
      sz_++;
    }

    void push_front(T&& value)
    {
      Elem* nw = new Elem(std::move(value), head_);
      if (empty()) {
        tail_ = nw;
      }
      head_ = nw;
      sz_++;
    }

    void pop_front()
    {
      if (empty()) {
        return;
      }
      if (head_ == tail_) {
        delete head_;
        head_ = nullptr;
        tail_ = nullptr;
      } else {
        Elem* tmp = head_->next_;
        delete head_;
        head_ = tmp;
      }
      sz_--;
    }

    void push_back(const T& value)
    {
      if (empty()) {
        head_ = new Elem(value);
        tail_ = head_;
      } else {
        tail_->next_ = new Elem(value);
        tail_ = tail_->next_;
      }
      sz_++;
    }

    void push_back(T&& value)
    {
      if (empty()) {
        head_ = new Elem(std::move(value));
        tail_ = head_;
      } else {
        tail_->next_ = new Elem(std::move(value));
        tail_ = tail_->next_;
      }
      sz_++;
    }

    void pop_back()
    {
      if (empty()) {
        return;
      }
      if (head_ == tail_) {
        delete head_;
        head_ = nullptr;
        tail_ = nullptr;
      } else {
        Elem* tmp = head_;
        while (tmp->next_ != tail_) {
          tmp = tmp->next_;
        }
        tmp->next_ = nullptr;
        delete tail_;
        tail_ = tmp;
      }
      sz_--;
    }

    Iter< T > insert_after(Iter< T > pos, const T& value);
    Iter< T > insert_after(Iter< T > pos, T&& value);
    Iter< T > erase_after(Iter< T > pos);

    void clear()
    {
      while (head_ != nullptr) {
        pop_front();
      }
    }
  };

  template < class T >
  void sum(T& a, const T& b)
  {
    if (std::numeric_limits<T>::max() - b < a) {
      throw std::overflow_error("Overflow");
    }
    a += b;
  }
}

#include "iter.hpp"
#include "citer.hpp"

#endif
