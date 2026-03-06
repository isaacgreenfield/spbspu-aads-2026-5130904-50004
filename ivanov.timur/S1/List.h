#ifndef LIST_H
#define LIST_H

#include <stdexcept>

template <class T> class List;

template <class T>
class Iter {
    friend class List<T>;

public:
    Iter() noexcept : ptr(nullptr) {}
    Iter(const Iter&) noexcept = default;
    Iter(Iter&&) noexcept = default;
    ~Iter() = default;
    Iter& operator=(const Iter&) noexcept = default;
    Iter& operator=(Iter&&) noexcept = default;


    T& operator*() const noexcept {
        return ptr->data_;
    }
    T* operator->() const noexcept {
        return &(ptr->data_);
    }

    Iter& operator++() noexcept {
        ptr = ptr->next_;
        return *this;
    }

    bool operator==(const Iter& other) const noexcept {
        return ptr == other.ptr;
    }
    bool operator!=(const Iter& other) const noexcept {
        return ptr != other.ptr;
    }

private:
    typename List<T>::Elem* ptr;
    explicit Iter(typename List<T>::Elem* p) noexcept : ptr(p) {}
};

template <typename T>
class List {
private:
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
    List() noexcept : head(nullptr), tail(nullptr), sz(0) {}

    ~List() {
        clear();
    }

    List(const List& other) : head(nullptr), tail(nullptr), sz(0) {
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

    Iter<T> begin() noexcept {
        return Iter<T>(head);
    };
    Iter<T> end() noexcept {
        return Iter<T>(tail);
    };

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
        head = nw;
    };
    void push_front(T&& value) {
        Elem* nw = new Elem(value, head);
        head = nw;
    };
    void pop_front() {
        if (head == tail) {
            delete head;
            head = nullptr;
            tail = nullptr;
        } else {
            Elem* tmp = head->next;
            delete head;
            head = tmp;
        }
    };

    void push_back(const T& value) {
        tail->next = new Elem(value);
        tail = tail->next;
    };
    void push_back(T&& value) {
        tail->next = new Elem(value);
        tail = tail->next;
    };
    void pop_back() {
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
    };

    Iter<T> insert_after(Iter<T> pos, const T& value);
    Iter<T> insert_after(Iter<T> pos, T&& value);
    Iter<T> erase_after(Iter<T> pos);
    void clear();
};

#endif
