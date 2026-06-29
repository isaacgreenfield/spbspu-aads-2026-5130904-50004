#ifndef S3HASHTABLE_H
#define S3HASHTABLE_H

#include "ix-vector.h"
#include <functional>
#include <stdexcept>
#include <utility>

namespace ivanov {

template <class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class HashTable {
public:
  struct Slot {
    Key key;
    Value value;
    int state = 0;
  };

  class iterator {
  public:
    iterator(Slot* p, Slot* e):
    ptr(p),
    end(e)
    {
      skip_empty();
    }
    std::pair< const Key&, Value& > operator*() const
    {
      return { ptr->key, ptr->value };
    }
    iterator& operator++()
    {
      ++ptr;
      skip_empty();
      return *this;
    }
    bool operator==(const iterator& other) const
    {
      return ptr == other.ptr;
    }
    bool operator!=(const iterator& other) const
    {
      return ptr != other.ptr;
    }
  private:
    friend class HashTable;
    friend class const_iterator;
    Slot* ptr;
    Slot* end;
    void skip_empty()
    {
      while (ptr != end && ptr->state != 1) ++ptr;
    }
  };

  class const_iterator {
  public:
    const_iterator(const Slot* p, const Slot* e):
    ptr(p),
    end(e)
    {
      skip_empty();
    }
    std::pair< const Key&, const Value& > operator*() const
    {
      return { ptr->key, ptr->value };
    }
    const_iterator& operator++()
    {
      ++ptr;
      skip_empty();
      return *this;
    }
    bool operator==(const const_iterator& other) const
    {
      return ptr == other.ptr;
    }
    bool operator!=(const const_iterator& other) const
    {
      return ptr != other.ptr;
    }
  private:
    friend class HashTable;
    friend class iterator;
    const Slot* ptr;
    const Slot* end;
    void skip_empty()
    {
      while (ptr != end && ptr->state != 1) ++ptr;
    }
  };

  static constexpr size_t DEFAULT_CAP = 16;

  HashTable():
  table(DEFAULT_CAP)
  {}
  HashTable(const HashTable&) = default;
  HashTable& operator=(const HashTable& other)
  {
    if (this != &other) {
      std::swap(table, other.table);
      std::swap(count, other.count);
      std::swap(hash, other.hash);
      std::swap(equal, other.equal);
    }
    return *this;
  }
  ~HashTable() = default;

  size_t size() const noexcept
  {
    return count;
  }
  size_t capacity() const noexcept
  {
    return table.size();
  }
  bool is_empty() const noexcept
  {
    return count == 0;
  }

  Value& at(const Key& key)
  {
    size_t idx = find_slot(key);
    if (idx == table.size()) {
      throw std::out_of_range("HashTable::at");
    }
    return table[idx].value;
  }

  const Value& at(const Key& key) const
  {
    size_t idx = find_slot(key);
    if (idx == table.size()) {
      throw std::out_of_range("HashTable::at");
    }
    return table[idx].value;
  }

  void add(const Key& key, const Value& value)
  {
    size_t existing = find_slot(key);
    if (existing != table.size()) {
      table[existing].value = value;
      return;
    }
    size_t idx = find_insert_slot(key);
    if (idx == table.size()) {
      rehash(capacity() * 2);
      idx = find_insert_slot(key);
    }
    table[idx].state = 1;
    table[idx].key = key;
    table[idx].value = value;
    ++count;
  }

  bool has(const Key& key) const
  {
    if (is_empty()) {
      return false;
    }
    return find_slot(key) != table.size();
  }

  Value drop(const Key& key)
  {
    size_t idx = find_slot(key);
    if (idx == table.size()) {
      throw std::out_of_range("HashTable::drop: key not found");
    }
    Value val = std::move(table[idx].value);
    table[idx].state = 2;
    --count;
    return val;
  }

  void clear() noexcept
  {
    for (auto& s : table) s.state = 0;
    count = 0;
  }

  void rehash(size_t new_cap)
  {
    if (new_cap < DEFAULT_CAP) {
      new_cap = DEFAULT_CAP;
    }
    idx::vector<Slot> new_table(new_cap);
    for (const auto& slot : table) {
      if (slot.state == 1) {
        size_t h = hash(slot.key) % new_table.size();
        size_t i = 0;
        while (true) {
          size_t idx = (h + i * i) % new_table.size();
          if (new_table[idx].state != 1) {
            new_table[idx] = slot;
            break;
          }
          ++i;
        }
      }
    }
    table.swap(new_table);
  }

  iterator begin()
  {
    return iterator(table.data(), table.data() + table.size());
  }
  iterator end()
  {
    return iterator(table.data() + table.size(), table.data() + table.size());
  }
  const_iterator begin() const
  {
    return cbegin();
  }
  const_iterator end() const
  {
    return cend();
  }
  const_iterator cbegin() const
  {
    return const_iterator(table.data(), table.data() + table.size());
  }
  const_iterator cend() const
  {
    return const_iterator(table.data() + table.size(), table.data() + table.size());
  }

private:
  idx::vector< Slot > table;
  size_t count = 0;
  Hash hash;
  Equal equal;

  size_t find_slot(const Key& key) const
  {
    if (table.size() == 0) {
      return table.size();
    }
    size_t h = hash(key) % table.size();
    for (size_t i = 0; i < table.size(); ++i) {
      size_t idx = (h + i * i) % table.size();
      if (table[idx].state == 0) {
        return table.size();
      }
      if (table[idx].state == 1 && equal(table[idx].key, key)) {
        return idx;
      }
    }
    return table.size();
  }

  size_t find_insert_slot(const Key& key) const
  {
    if (table.size() == 0) {
      return table.size();
    }
    size_t h = hash(key) % table.size();
    for (size_t i = 0; i < table.size(); ++i) {
      size_t idx = (h + i * i) % table.size();
      if (table[idx].state != 1) {
        return idx;
      }
    }
    return table.size();
  }
};

}

#endif