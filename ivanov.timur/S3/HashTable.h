#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

template<class Key, class Value, class Hash, class Equal>
class HashTable {
  struct Slot {
    Key key;
    Value value;
    int state = 0; // 0 – пусто, 1 – занято, 2 – tombstone
  };

  std::vector<Slot> table;
  size_t count = 0;
  Hash hash;
  Equal equal;

  size_t probe(size_t hash_f, size_t i) const noexcept {
    return (hash_f + i * i) % table.size();
  }

  size_t find_slot(const Key &key) const;
  size_t find_insert_slot(const Key &key);
  void rehash_internal(size_t new_cap);

public:
  void add(const Key &key, const Value &value);

  bool has(const Key &key) const;

  Value drop(const Key &key);

  void rehash(size_t new_slots);

  size_t size() const noexcept { return count; }
  size_t capacity() const noexcept { return table.size(); }
  bool empty() const noexcept { return count == 0; }

  void clear() noexcept;

  class iterator {
    Slot *ptr;
    Slot *end;

    void skip_empty() {
      while (ptr != end && ptr->state != 1) ++ptr;
    }

  public:
    iterator(Slot *p, Slot *e) : ptr(p), end(e) { skip_empty(); }

    std::pair<const Key &, Value &> operator*() const;

    iterator &operator++();

    bool operator==(const iterator &other) const;

    bool operator!=(const iterator &other) const;
  };

  class const_iterator {
    const Slot *ptr;
    const Slot *end;

    void skip_empty() {
      while (ptr != end && ptr->state != 1) ++ptr;
    }

  public:
    const_iterator(const Slot *p, const Slot *e) : ptr(p), end(e) { skip_empty(); }

    std::pair<const Key &, const Value &> operator*() const;

    const_iterator &operator++();

    bool operator==(const const_iterator &other) const;

    bool operator!=(const const_iterator &other) const;
  };

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

  HashTable();

  HashTable(const HashTable &other);

  ~HashTable() = default;
};

template<class Key, class Value, class Hash, class Equal>
size_t HashTable<Key, Value, Hash, Equal>::find_slot(const Key &key) const {
  if (table.empty()) return 0;
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

template<class Key, class Value, class Hash, class Equal>
size_t HashTable<Key, Value, Hash, Equal>::find_insert_slot(const Key &key) {
  size_t h = hash(key) % table.size();
  for (size_t i = 0; i < table.size(); ++i) {
    size_t idx = probe(h, i);
    if (table[idx].state == 0 || table[idx].state == 2) return idx;
    else continue;
  }
  throw std::overflow_error("Hash Table Overflow");
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::rehash_internal(size_t new_cap) {
  try {
    std::vector<Slot> new_table;
    for (size_t i = 0; i < capacity(); ++i) {

    }
  } catch (...) {
    throw std::bad_alloc();
  }
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::add(const Key &key, const Value &value) {
}

template<class Key, class Value, class Hash, class Equal>
bool HashTable<Key, Value, Hash, Equal>::has(const Key &key) const {
}

template<class Key, class Value, class Hash, class Equal>
Value HashTable<Key, Value, Hash, Equal>::drop(const Key &key) {
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::rehash(size_t new_slots) {
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::clear() noexcept {
}

#endif
