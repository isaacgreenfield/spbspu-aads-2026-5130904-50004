#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>
#include <functional>
#include <limits>

namespace ivanov
{
  template< class T >
  class Iter;
  template< class T >
  class CIter;

  template< class T >
  class List
  {
    friend class Iter< T >;
    friend class CIter< T >;

  protected:
    struct Elem
    {
      T data_;
      Elem* next_;

      explicit Elem(const T& val, Elem* nxt = nullptr):
          data_(val),
          next_(nxt)
      {}

      explicit Elem(T&& val, Elem* nxt = nullptr):
          data_(std::move(val)),
          next_(nxt)
      {}
    };

    Elem* head_;
    Elem* tail_;
    size_t sz_;

  public:
    List() noexcept
      : head_(nullptr),
        tail_(nullptr),
        sz_(0)
    {}

    ~List()
    {
      clear();
    }

    List(const List& other):
        head_(nullptr),
        tail_(nullptr),
        sz_(0)
    {
      for (Elem* curr = other.head_; curr != nullptr; curr = curr->next_) {
        push_back(curr->data_);
      }
    }

    List(List&& other) noexcept:
        head_(other.head_),
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

    Elem* find_prev(List& list, Elem* target) noexcept;

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

    void splice(Iter pos, List& other) noexcept;

    void splice(Iter pos, List& other, Iter it) noexcept;

    void splice(Iter pos, List& other, Iter first, Iter last) noexcept;

    void sort() noexcept(noexcept(std::declval< T >() < std::declval< T >())) {
      sort(std::less< T >());
    }

    template <typename Compare>
    void sort(Compare comp) noexcept(noexcept(comp(std::declval< T >(), std::declval< T >())));

    void merge(List& other) noexcept(noexcept(std::declval< T >() < std::declval< T >())) {
      merge(other, std::less< T >());
    
    }
    template <typename Compare>
    void merge(List& other, Compare comp) noexcept(noexcept(comp(std::declval< T >(), std::declval< T >())));

    template <typename Up>
    void partition(Up pred) noexcept(noexcept(pred(std::declval< T >())));

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

  template< class T >
  void sum(T& a, const T& b)
  {
    if (std::numeric_limits< T >::max() - b < a) {
      throw std::overflow_error("Overflow");
    }
    a += b;
  }

  template <typename T>
  typename List< T >::Elem* find_prev(List< T >& list, typename List< T >::Elem* target) noexcept {
    if (target == list.head_) return nullptr;
    auto curr = list.head_;
    while (curr && curr->next_ != target) curr = curr->next_;
    return curr;
  }

  template <typename T>
  void List< T >::splice(Iter pos, List& other) noexcept {
    if (other.empty()) return;
    Elem* first = other.head_;
    Elem* last = other.tail_;
    size_t n = other.sz_;

    other.head_ = other.tail_ = nullptr;
    other.sz_ = 0;

    if (empty() && pos == end()) {
        head_ = first;
        tail_ = last;
        sz_ = n;
        return;
    }
    Elem* after = pos.ptr_;
    if (after == nullptr) return;
    last->next_ = after->next_;
    after->next_ = first;
    if (after == tail_) tail_ = last;
    sz_ += n;
  }

  template <typename T>
  void List< T >::splice(Iter pos, List& other, Iter it) noexcept {
    if (it == other.end()) return;
    Elem* node = it.ptr_;
    Elem* prev = find_prev(other, node);

    if (prev) {
        prev->next_ = node->next_;
    } else {
        other.head_ = node->next_;
    }
    if (other.tail_ == node) {
        other.tail_ = prev;
    }
    --other.sz_;

    if (empty() && pos == end()) {
        head_ = tail_ = node;
        node->next_ = nullptr;
        sz_ = 1;
        return;
    }
    Elem* after = pos.ptr_;
    if (after == nullptr) return;
    node->next_ = after->next_;
    after->next_ = node;
    if (after == tail_) tail_ = node;
    ++sz_;
  }

  template <typename T>
  void List< T >::splice(Iter pos, List& other, Iter first, Iter last) noexcept {
    if (first == last) return;
    Elem* first_node = first.ptr_;
    Elem* last_node = (last != other.end()) ? last.ptr_ : nullptr;
    Elem* prev_first = find_prev(other, first_node);
    Elem* prev_last = nullptr;
    size_t n = 0;
    Elem* chain_end = first_node;
    while (chain_end && chain_end != last_node) {
        prev_last = chain_end;
        chain_end = chain_end->next_;
        ++n;
    }
    if (chain_end != last_node) return;

    if (prev_first) {
        prev_first->next_ = last_node;
    } else {
        other.head_ = last_node;
    }
    if (other.tail_ == prev_last) {
        other.tail_ = prev_first;
    }
    other.sz_ -= n;

    if (empty() && pos == end()) {
        head_ = first_node;
        tail_ = prev_last;
        prev_last->next_ = nullptr;
        sz_ = n;
        return;
    }
    Elem* after = pos.ptr_;
    if (after == nullptr) return;
    prev_last->next_ = after->next_;
    after->next_ = first_node;
    if (after == tail_) tail_ = prev_last;
    sz_ += n;
  }

  template <typename T>
  template <typename Compare>
  void List< T >::merge(List& other, Compare comp) noexcept(noexcept(comp(std::declval< T >(), std::declval< T >()))) {
    if (this == &other || other.empty()) return;
    if (empty()) {
        swap(other);
        return;
    }

    Elem* cur1 = head_;
    Elem* cur2 = other.head_;
    Elem* prev1 = nullptr;
    size_t other_sz = other.sz_;

    while (cur1 && cur2) {
        if (comp(cur2->data_, cur1->data_)) {
            Elem* next2 = cur2->next_;
            if (prev1) {
                prev1->next_ = cur2;
            } else {
                head_ = cur2;
            }
            cur2->next_ = cur1;
            prev1 = cur2;
            cur2 = next2;
            --other_sz;
        } else {
            prev1 = cur1;
            cur1 = cur1->next_;
        }
    }
    if (cur2) {
        if (prev1) {
            prev1->next_ = cur2;
        } else {
            head_ = cur2;
        }
        tail_ = other.tail_;
        sz_ += other_sz;
    } else {
        sz_ += other.sz_ - other_sz;
        tail_ = (tail_ == nullptr ? prev1 : tail_);
    }

    other.head_ = other.tail_ = nullptr;
    other.sz_ = 0;
  }

  template <typename T>
  template <typename Compare>
  void List< T >::sort(Compare comp) noexcept(noexcept(comp(std::declval< T >(), std::declval< T >()))) {
    if (sz_ <= 1) return;

    List< T > right;
    {
        Elem* slow = head_;
        Elem* fast = head_->next_;
        while (fast && fast->next_) {
            slow = slow->next_;
            fast = fast->next_->next_;
        }
        right.head_ = slow->next_;
        right.tail_ = tail_;
        right.sz_ = sz_ / 2;
        slow->next_ = nullptr;
        tail_ = slow;
        sz_ -= right.sz_;
    }
    sort(comp);
    right.sort(comp);
    merge(right, comp);
  }

  template <typename T>
  template <typename Up>
  void List< T >::partition(Up pred) noexcept(noexcept(pred(std::declval< T >()))) {
    List< T > true_list, false_list;
    Elem* prev = nullptr;
    Elem* curr = head_;
    while (curr) {
        Elem* next = curr->next_;
        if (pred(curr->data_)) {
            if (prev) prev->next_ = next;
            else head_ = next;
            if (tail_ == curr) tail_ = prev;
            curr->next_ = nullptr;
            if (!true_list.tail_) {
                true_list.head_ = true_list.tail_ = curr;
            } else {
                true_list.tail_->next_ = curr;
                true_list.tail_ = curr;
            }
            ++true_list.sz_;
        } else {
            if (prev) prev->next_ = next; else head_ = next;
            if (tail_ == curr) tail_ = prev;
            curr->next_ = nullptr;
            if (!false_list.tail_) {
                false_list.head_ = false_list.tail_ = curr;
            } else {
                false_list.tail_->next_ = curr;
                false_list.tail_ = curr;
            }
            ++false_list.sz_;
        }
        curr = next;
    }
    sz_ = 0;
    if (!true_list.empty()) {
        true_list.tail_->next_ = false_list.head_;
        if (!false_list.empty()) true_list.tail_ = false_list.tail_;
        swap(true_list);
    } else {
        swap(false_list);
    }
  }
}

#include "iter.hpp"
#include "citer.hpp"

#endif
