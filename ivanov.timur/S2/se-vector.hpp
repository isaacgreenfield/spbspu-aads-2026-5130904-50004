#ifndef SE_VECTOR_H
#define SE_VECTOR_H
#include <cstddef>
#include <stdexcept>
#include <initializer_list>
#include <utility>

namespace knk {
    template <class T>
    class Vector {
    public:
        using iterator = T*;
        using const_iterator = const T*;

        ~Vector();
        Vector();
        Vector(size_t size, const T& value);
        Vector(std::initializer_list<T> init);
        explicit Vector(size_t size);

        Vector(const Vector<T>& rhs);
        Vector(Vector<T>&& rhs) noexcept;
        Vector operator=(Vector<T>&& rhs);

        void swap(Vector<T>& rhs);

        bool isEmpty() const noexcept;
        size_t getSize() const noexcept;
        size_t getCapacity() const noexcept;

        void pushBack(const T& elem);
        void pushFront(const T& elem);
        void popBack();
        void resize();

        T& operator[](size_t id);
        const T& operator[](size_t id) const;

        T& get(size_t id) noexcept;
        const T& get(size_t id) const noexcept;
        T& at(size_t id);
        const T& at(size_t id) const;

        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;

        iterator insert(const_iterator pos, const T& value);
        iterator erase(const_iterator pos);

    private:
        T* data_;
        size_t size_;
        size_t capacity_;
    };
}

template<class T>
knk::Vector<T>::~Vector() {
    delete[] data_;
}

template<class T>
knk::Vector<T>::Vector() : Vector(0) {}

template<class T>
knk::Vector<T>::Vector(size_t size, const T& value) : Vector(size) {
    try {
        for (size_t i = 0; i < size; ++i)
            data_[i] = value;
    } catch (...) {
        delete[] data_;
        throw;
    }
}

template<class T>
knk::Vector<T>::Vector(std::initializer_list<T> init) : Vector() {
    for (const auto& elem : init)
        pushBack(elem);
}

template<class T>
knk::Vector<T>::Vector(size_t size)
    : data_(size ? new T[2 * size] : nullptr),
      size_(size),
      capacity_(size ? 2 * size : 0)
{}

template<class T>
knk::Vector<T>::Vector(const Vector<T>& rhs) : Vector(rhs.getSize()) {
    for (size_t i = 0; i < rhs.getSize(); ++i)
        data_[i] = rhs.data_[i];
}

template<class T>
knk::Vector<T>& knk::Vector<T>::operator=(const Vector<T> rhs) {
    swap(rhs);
    return *this;
}

template<class T>
knk::Vector<T>::Vector(Vector<T>&& rhs) noexcept
    : data_(rhs.data_), size_(rhs.size_), capacity_(rhs.capacity_) {
    rhs.data_ = nullptr;
    rhs.size_ = 0;
    rhs.capacity_ = 0;
}

template<class T>
knk::Vector<T> knk::Vector<T>::operator=(Vector<T>&& rhs) {
    Vector<T> cpy(std::move(rhs));
    swap(cpy);
    return *this;
}

template<class T>
void knk::Vector<T>::swap(Vector<T>& rhs) {
    std::swap(data_, rhs.data_);
    std::swap(size_, rhs.size_);
    std::swap(capacity_, rhs.capacity_);
}

template<class T>
bool knk::Vector<T>::isEmpty() const noexcept {
    return !getSize();
}

template<class T>
size_t knk::Vector<T>::getSize() const noexcept {
    return size_;
}

template<class T>
size_t knk::Vector<T>::getCapacity() const noexcept {
    return capacity_;
}

template<class T>
void knk::Vector<T>::pushBack(const T& elem) {
    if (size_ == capacity_) {
        size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < size_; ++i)
            new_data[i] = data_[i];
        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
    }
    data_[size_] = elem;
    ++size_;
}

template<class T>
void knk::Vector<T>::pushFront(const T& elem) {
    if (size_ == capacity_) {
        size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        T* new_data = new T[new_cap];
        new_data[0] = elem;
        for (size_t i = 0; i < size_; ++i)
            new_data[i + 1] = data_[i];
        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
        ++size_;
    } else {
        // сдвиг вправо
        for (size_t i = size_; i > 0; --i)
            data_[i] = data_[i - 1];
        data_[0] = elem;
        ++size_;
    }
}

template<class T>
void knk::Vector<T>::popBack() {
    if (isEmpty())
        throw std::logic_error("Cannot pop from empty vector");
    --size_;
}

template<class T>
void knk::Vector<T>::resize() {
    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    T* new_data = new T[new_capacity];
    for (size_t i = 0; i < size_; ++i)
        new_data[i] = data_[i];
    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
}

template<class T>
T& knk::Vector<T>::operator[](size_t id) {
    return data_[id];
}

template<class T>
const T& knk::Vector<T>::operator[](size_t id) const {
    return data_[id];
}

template<class T>
T& knk::Vector<T>::get(size_t id) noexcept {
    return const_cast<T&>(static_cast<const Vector<T>*>(this)->get(id));
}

template<class T>
const T& knk::Vector<T>::get(size_t id) const noexcept {
    return data_[id];
}

template<class T>
T& knk::Vector<T>::at(size_t id) {
    const Vector<T>* cthis = this;
    const T& cr = cthis->at(id);
    return const_cast<T&>(cr);
}

template<class T>
const T& knk::Vector<T>::at(size_t id) const {
    if (id < getSize())
        return get(id);
    throw std::logic_error("at: index out of range");
}

template<class T>
typename knk::Vector<T>::iterator knk::Vector<T>::begin() {
    return data_;
}

template<class T>
typename knk::Vector<T>::const_iterator knk::Vector<T>::begin() const {
    return data_;
}

template<class T>
typename knk::Vector<T>::iterator knk::Vector<T>::end() {
    return data_ + size_;
}

template<class T>
typename knk::Vector<T>::const_iterator knk::Vector<T>::end() const {
    return data_ + size_;
}

template<class T>
typename knk::Vector<T>::const_iterator knk::Vector<T>::cbegin() const {
    return begin();
}

template<class T>
typename knk::Vector<T>::const_iterator knk::Vector<T>::cend() const {
    return end();
}

template<class T>
typename knk::Vector<T>::iterator knk::Vector<T>::insert(const_iterator pos, const T& value) {
    size_t idx = pos - data_;
    if (idx > size_)
        throw std::logic_error("insert position out of range");

    if (size_ == capacity_) {
        size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < idx; ++i)
            new_data[i] = data_[i];
        new_data[idx] = value;
        for (size_t i = idx; i < size_; ++i)
            new_data[i + 1] = data_[i];
        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
        ++size_;
        return data_ + idx;
    } else {
        for (size_t i = size_; i > idx; --i)
            data_[i] = data_[i - 1];
        data_[idx] = value;
        ++size_;
        return data_ + idx;
    }
}

template<class T>
typename knk::Vector<T>::iterator knk::Vector<T>::erase(const_iterator pos) {
    size_t idx = pos - data_;
    if (idx >= size_)
        throw std::logic_error("erase position out of range");
    // сдвиг влево
    for (size_t i = idx; i < size_ - 1; ++i)
        data_[i] = data_[i + 1];
    --size_;
    return data_ + idx;
}

#endif