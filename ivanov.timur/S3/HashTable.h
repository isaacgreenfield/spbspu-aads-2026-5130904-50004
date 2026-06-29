#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <functional>
#include <stdexcept>
#include "idx-vector.h"

namespace xtra {
  template <typename Key, typename Value>
  struct Slot
  {
      Key key;
      Value value;
      int state = 0;
  };

  template <typename Key, typename Value>
  class iterator
  {
    public:
      iterator(Slot<Key, Value> *p, Slot<Key, Value> *e):
        ptr(p),
        end(e)
      {
        skip_is_empty();
      }
      std::pair< const Key &, Value & > operator*() const;
      iterator &operator++();

      bool operator==(const iterator &other) const;
      bool operator!=(const iterator &other) const;

    private:
      friend class const_iterator;
      Slot<Key, Value> *ptr;
      Slot<Key, Value> *end;
      void skip_is_empty()
      {
        while (ptr != end && ptr->state != 1) {
          ++ptr;
        }
      }
  };

  template <typename Key, typename Value>
  class const_iterator
  {
    public:
      const_iterator(const Slot<Key, Value> *p, const Slot<Key, Value> *e):
        ptr(p),
        end(e)
      {
        skip_is_empty();
      }

      std::pair< const Key &, const Value & > operator*() const;
      const_iterator &operator++();

      bool operator==(const const_iterator &other) const;
      bool operator!=(const const_iterator &other) const;

    private:
      friend class iterator<Key, Value>;
      const Slot<Key, Value> *ptr;
      const Slot<Key, Value> *end;
      void skip_is_empty()
      {
        while (ptr != end && ptr->state != 1) {
          ++ptr;
        }
      }
  };
}

namespace ivanov {
  template< class Key, class Value, class Hash, class Equal >
  class HashTable
  {
  public:
    void add(const Key &key, const Value &value);
    bool has(const Key &key) const;
    Value drop(const Key &key);
    void rehash(size_t new_cap);

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

    void clear() noexcept;

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

    xtra::iterator<Key, Value> begin();
    xtra::iterator<Key, Value> end();

    xtra::const_iterator<Key, Value> begin() const
    {
      return cbegin();
    }
    xtra::const_iterator<Key, Value> end() const
    {
      return cend();
    }
    xtra::const_iterator<Key, Value> cbegin() const;
    xtra::const_iterator<Key, Value> cend() const;

    const int magical_number = 16;
    HashTable(): table()
    {
      for (size_t i = 0; i < magical_number; ++i) {
        table.emplace_back();
      }
    }
    HashTable(const HashTable &other) = default;
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
  private:
    friend struct xtra::Slot<Key, Value>;
    friend class xtra::iterator<Key, Value>;
    friend class xtra::const_iterator<Key, Value>;
    idx::vector< xtra::Slot<Key, Value> > table;
    size_t count = 0;
    Hash hash;
    Equal equal;

    xtra::Slot<Key, Value>* data() noexcept
    {
      return table.size() > 0 ? &table[0] : nullptr;
    }
    const xtra::Slot<Key, Value>* data() const noexcept
    {
      return table.size() > 0 ? &table[0] : nullptr;
    }

    size_t probe(size_t hash_f, size_t i) const noexcept
    {
      return (hash_f + i * i) % table.size();
    }
    size_t find_slot(const Key &key) const;
    size_t find_insert_slot(const Key &key);
  };
}

using ivanov::HashTable;

template< class Key, class Value, class Hash, class Equal >
size_t HashTable< Key, Value, Hash, Equal >::find_slot(const Key &key) const
{
  if (is_empty()) {
    return table.size();
  }
  size_t h = hash(key) % table.size();
  for (size_t i = 0; i < table.size(); ++i) {
    size_t idx = probe(h, i);
    if (table[idx].state == 0) {
      return table.size();
    }
    if (table[idx].state == 1 && equal(table[idx].key, key)) {
      return idx;
    }
  }
  return table.size();
}

template< class Key, class Value, class Hash, class Equal >
size_t HashTable< Key, Value, Hash, Equal >::find_insert_slot(const Key &key)
{
  size_t h = hash(key) % table.size();
  for (size_t i = 0; i < table.size(); ++i) {
    size_t idx = probe(h, i);
    if (table[idx].state == 0 || table[idx].state == 2) {
      return idx;
    }
    else {
      continue;
    }
  }
  throw std::overflow_error("Hash Table Overflow");
}

template< class Key, class Value, class Hash, class Equal >
void HashTable< Key, Value, Hash, Equal >::rehash(size_t new_cap)
{
  new_cap = (new_cap > 16)? new_cap : 16;
  idx::vector< xtra::Slot<Key, Value> > new_table;
  for (size_t i = 0; i < new_cap; ++i) {
    new_table.emplace_back();
  }
  size_t new_count = 0;
  for (const auto &slot: table) {
    if (slot.state == 1) {
      size_t h = hash(slot.key) % new_table.size();
      size_t i = 0;
      while (true) {
        size_t idx = probe(h, i++);
        if (new_table[idx].state != 1) {
          new_table[idx].key = slot.key;
          new_table[idx].value = slot.value;
          new_table[idx].state = 1;
          ++new_count;
          break;
        }
      }
    }
  }
  table = std::move(new_table);
  count = new_count;
}

template< class Key, class Value, class Hash, class Equal >
void HashTable< Key, Value, Hash, Equal >::add(const Key &key, const Value &value)
{
  size_t idx = find_insert_slot(key);
  table[idx].state = 1;
  table[idx].key = key;
  table[idx].value = value;
  count++;
}

template< class Key, class Value, class Hash, class Equal >
bool HashTable< Key, Value, Hash, Equal >::has(const Key &key) const
{
  if (is_empty()) {
    return false;
  }
  size_t idx = find_slot(key);
  return idx != table.size();
}

template< class Key, class Value, class Hash, class Equal >
Value HashTable< Key, Value, Hash, Equal >::drop(const Key &key)
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

template< class Key, class Value, class Hash, class Equal >
void HashTable< Key, Value, Hash, Equal >::clear() noexcept
{
  for (auto &s : table) {
    s.state = 0;
  }
  count = 0;
}

template< class Key, class Value, class Hash, class Equal >
std::pair< const Key &, Value & > xtra::iterator<Key, Value>::operator*() const
{
  return { ptr->key, ptr->value };
}

template< class Key, class Value, class Hash, class Equal >
typename xtra::iterator<Key, Value> & xtra::iterator<Key, Value>::operator++()
{
  ++ptr;
  skip_is_empty();
  return *this;
}

template< class Key, class Value, class Hash, class Equal >
std::pair< const Key &, const Value & > xtra::const_iterator<Key, Value>::operator*() const {
  return { ptr->key, ptr->value };
}

template< class Key, class Value, class Hash, class Equal >
typename xtra::const_iterator<Key, Value> & xtra::const_iterator<Key, Value>::operator++()
{
  ++ptr;
  skip_is_empty();
  return *this;
}

template< class Key, class Value, class Hash, class Equal >
typename xtra::iterator<Key, Value> HashTable< Key, Value, Hash, Equal >::begin()
{
  return xtra::iterator<Key, Value>(data(), data() + table.size());
}
template< class Key, class Value, class Hash, class Equal >
typename xtra::iterator<Key, Value> HashTable< Key, Value, Hash, Equal >::end()
{
  return xtra::iterator<Key, Value>(data() + table.size(), data() + table.size());
}

template< class Key, class Value, class Hash, class Equal >
typename xtra::const_iterator<Key, Value> HashTable< Key, Value, Hash, Equal >::cbegin() const
{
  return xtra::const_iterator<Key, Value>(data(), data() + table.size());
}
template< class Key, class Value, class Hash, class Equal >
typename xtra::const_iterator<Key, Value> HashTable< Key, Value, Hash, Equal >::cend() const
{
  return xtra::const_iterator<Key, Value>(data() + table.size(), data() + table.size());
}

template <typename Key, typename Value>
bool xtra::iterator<Key, Value>::operator==(const iterator &other) const
{
  return ptr == other.ptr;
}
template <typename Key, typename Value>
bool xtra::iterator<Key, Value>::operator!=(const iterator &other) const
{
  return ptr != other.ptr;
}
template <typename Key, typename Value>
bool xtra::const_iterator<Key, Value>::operator==(const const_iterator &other) const
{
  return ptr == other.ptr;
}
template <typename Key, typename Value>
bool xtra::const_iterator<Key, Value>::operator!=(const const_iterator &other) const
{
  return ptr != other.ptr;
}

#endif