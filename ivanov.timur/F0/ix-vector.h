#ifndef IX_VECTOR_H
#define IX_VECTOR_H

#include <utility>

namespace idx {

template <typename T>
class vector {
private:
  T* data_ = nullptr;
  size_t size_ = 0;
  size_t cap_ = 0;

  static T* allocate(size_t n) {
    return reinterpret_cast<T*>(new char[n * sizeof(T)]);
  }

  static void deallocate(T* p) {
    delete[] reinterpret_cast<char*>(p);
  }

public:
  vector() = default;

  explicit vector(const T& value) {
    size_ = cap_ = 1;
    data_ = allocate(1);
    new (data_) T(value);
  }

  ~vector() {
    for (size_t i = 0; i < size_; ++i)
      data_[i].~T();
    deallocate(data_);
  }

  void clear() {
    for (size_t i = 0; i < size_; ++i)
      data_[i].~T();
    size_ = 0;
  }

  bool empty() const { return size_ == 0; }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (size_ == cap_) {
      size_t new_cap = (cap_ == 0) ? 4 : cap_ * 2;
      T* new_data = allocate(new_cap);
      size_t i = 0;
      try {
        for (; i < size_; ++i)
          new (new_data + i) T(data_[i]);
      } catch (...) {
        for (size_t j = 0; j < i; ++j)
          new_data[j].~T();
        deallocate(new_data);
        throw;
      }
      for (size_t j = 0; j < size_; ++j)
        data_[j].~T();
      deallocate(data_);
      data_ = new_data;
      cap_ = new_cap;
    }
    new (data_ + size_) T(std::forward<Args>(args)...);
    ++size_;
  }

  class iterator {
    friend class vector;
    T* ptr_;
    explicit iterator(T* p) : ptr_(p) {}
  public:
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    iterator& operator++() { ++ptr_; return *this; }
    iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
    bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
    iterator operator+(size_t n) const { return iterator(ptr_ + n); }
    size_t operator-(const iterator& other) const { return ptr_ - other.ptr_; }
  };

  class const_iterator {
    friend class vector;
    const T* ptr_;
    explicit const_iterator(const T* p) : ptr_(p) {}
  public:
    const T& operator*() const { return *ptr_; }
    const T* operator->() const { return ptr_; }
    const_iterator& operator++() { ++ptr_; return *this; }
    const_iterator operator++(int) { const_iterator tmp = *this; ++ptr_; return tmp; }
    bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
  };

  iterator begin() { return iterator(data_); }
  idx::vector<int>::iterator begin() const { return const_iterator(data_); }
  iterator end() { return iterator(data_ + size_); }
  const_iterator end() const { return const_iterator(data_ + size_); }

  iterator insert(iterator pos, iterator first, iterator last) {
    size_t offset = pos.ptr_ - data_;
    size_t count = last.ptr_ - first.ptr_;
    if (count == 0) return pos;

    if (size_ + count > cap_) {
      size_t new_cap = cap_ * 2;
      while (new_cap < size_ + count) new_cap *= 2;
      T* new_data = allocate(new_cap);
      size_t i = 0;
      try {
        for (; i < offset; ++i)
          new (new_data + i) T(data_[i]);
        for (size_t j = 0; j < count; ++j, ++i)
          new (new_data + i) T(*(first.ptr_ + j));
        for (size_t j = offset; j < size_; ++j, ++i)
          new (new_data + i) T(data_[j]);
      } catch (...) {
        for (size_t j = 0; j < i; ++j)
          new_data[j].~T();
        deallocate(new_data);
        throw;
      }
      for (size_t j = 0; j < size_; ++j)
        data_[j].~T();
      deallocate(data_);
      data_ = new_data;
      cap_ = new_cap;
    } else {
      for (size_t i = size_; i > offset; --i) {
        new (data_ + i + count - 1) T(data_[i - 1]);
        data_[i - 1].~T();
      }
      for (size_t j = 0; j < count; ++j)
        new (data_ + offset + j) T(*(first.ptr_ + j));
    }
    size_ += count;
    return iterator(data_ + offset);
  }

  vector(const vector& other) {
    size_ = other.size_;
    cap_ = other.size_;
    if (cap_ > 0) {
      data_ = allocate(cap_);
      for (size_t i = 0; i < size_; ++i)
        new (data_ + i) T(other.data_[i]);
    }
  }

  vector& operator=(vector other) noexcept {
    std::swap(this->size_, other.size_);
    std::swap(this->cap_, other.cap_);
    std::swap(this->data_, other.data_);
    return *this;
  }

  size_t size() const { return size_; }

  const T& operator[](size_t i) const { return data_[i]; }
  T& operator[](size_t i) { return data_[i]; }

  void push_back(const T& value) {
    if (size_ == cap_) {
      size_t new_cap = (cap_ == 0) ? 4 : cap_ * 2;
      T* new_data = allocate(new_cap);
      size_t i = 0;
      try {
        for (; i < size_; ++i)
          new (new_data + i) T(data_[i]);
      } catch (...) {
        for (size_t j = 0; j < i; ++j)
          new_data[j].~T();
        deallocate(new_data);
        throw;
      }
      for (size_t j = 0; j < size_; ++j)
        data_[j].~T();
      deallocate(data_);
      data_ = new_data;
      cap_ = new_cap;
    }
    new (data_ + size_) T(value);
    ++size_;
  }

  void append(const vector& other) {
    if (other.size_ == 0) return;
    size_t new_size = size_ + other.size_;
    if (new_size > cap_) {
      size_t new_cap = (cap_ == 0) ? other.size_ : cap_ * 2;
      while (new_cap < new_size)
        new_cap *= 2;
      T* new_data = allocate(new_cap);
      size_t i = 0;
      try {
        for (; i < size_; ++i)
          new (new_data + i) T(data_[i]);
        for (size_t j = 0; j < other.size_; ++j, ++i)
          new (new_data + i) T(other.data_[j]);
      } catch (...) {
        for (size_t j = 0; j < i; ++j)
          new_data[j].~T();
        deallocate(new_data);
        throw;
      }
      for (size_t j = 0; j < size_; ++j)
        data_[j].~T();
      deallocate(data_);
      data_ = new_data;
      cap_ = new_cap;
    } else {
      for (size_t i = 0; i < other.size_; ++i)
        new (data_ + size_ + i) T(other.data_[i]);
    }
    size_ = new_size;
  }
};

}

#endif