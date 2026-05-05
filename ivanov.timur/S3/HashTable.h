#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <functional>
#include <stdexcept>
#include <vector>
namespace ivanov {
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

  public:
    void add(const Key &key, const Value &value);

    bool has(const Key &key) const;

    Value drop(const Key &key);

    void rehash(size_t new_cap);

    size_t size() const noexcept { return count; }
    size_t capacity() const noexcept { return table.size(); }
    bool empty() const noexcept { return count == 0; }

    void clear() noexcept;

    Value& at(const Key& key) {
      size_t idx = find_slot(key);
      if (idx == table.size()) throw std::out_of_range("HashTable::at");
      return table[idx].value;
    }

    const Value& at(const Key& key) const {
      size_t idx = find_slot(key);
      if (idx == table.size()) throw std::out_of_range("HashTable::at");
      return table[idx].value;
    }

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

      bool operator==(const iterator &other) const {return ptr == other.ptr; };

      bool operator!=(const iterator &other) const {return ptr != other.ptr; };
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

      bool operator==(const const_iterator &other) const {return ptr == other.ptr; };

      bool operator!=(const const_iterator &other) const {return ptr != other.ptr; };
    };

    iterator begin();
    iterator end();

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const;
    const_iterator cend() const;

    HashTable(): table(16) {};
    HashTable(const HashTable &other) = default;
    ~HashTable() = default;
  };
}

using ivanov::HashTable;

template<class Key, class Value, class Hash, class Equal>
size_t HashTable<Key, Value, Hash, Equal>::find_slot(const Key &key) const {
  if (table.empty()) return table.size();
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
void HashTable<Key, Value, Hash, Equal>::rehash(size_t new_cap) {
  new_cap = (new_cap > 16)? new_cap : 16;
  std::vector<Slot> new_table(new_cap);
  for (auto &s: new_table) {
    s.state = 0;
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
  table.swap(new_table);
  count = new_count;
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::add(const Key &key, const Value &value) {
  try {
    size_t idx = find_insert_slot(key);
    table[idx].state = 1;
    table[idx].key = key;
    table[idx].value = value;
    count++;
  } catch (...) {
      throw;
  }
}

template<class Key, class Value, class Hash, class Equal>
bool HashTable<Key, Value, Hash, Equal>::has(const Key &key) const {
  if (table.empty()) return false;
  size_t idx = find_slot(key);
  return idx != table.size();
}

template<class Key, class Value, class Hash, class Equal>
Value HashTable<Key, Value, Hash, Equal>::drop(const Key &key) {
  size_t idx = find_slot(key);
  if (idx == table.size()) {
    throw std::out_of_range("HashTable::drop: key not found");
  }
  Value val = std::move(table[idx].value);
  table[idx].state = 2;
  --count;
  return val;
}

template<class Key, class Value, class Hash, class Equal>
void HashTable<Key, Value, Hash, Equal>::clear() noexcept {
  for (auto &s : table) s.state = 0;
  count = 0;
}

template<class Key, class Value, class Hash, class Equal>
std::pair<const Key &, Value &> HashTable<Key, Value, Hash, Equal>::iterator::operator*() const {
  return { ptr->key, ptr->value };
}

template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::iterator & HashTable<Key, Value, Hash, Equal>::iterator::operator++() {
  ++ptr;
  skip_empty();
  return *this;
}

template<class Key, class Value, class Hash, class Equal>
std::pair<const Key &, const Value &> HashTable<Key, Value, Hash, Equal>::const_iterator::operator*() const {
  return { ptr->key, ptr->value };
}

template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::const_iterator & HashTable<Key, Value, Hash, Equal>::const_iterator::operator++() {
  ++ptr;
  skip_empty();
  return *this;
}

template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::iterator HashTable<Key, Value, Hash, Equal>::begin() {
  return iterator(table.data(), table.data() + table.size());
}
template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::iterator HashTable<Key, Value, Hash, Equal>::end() {
  return iterator(table.data() + table.size(), table.data() + table.size());
}

template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::const_iterator HashTable<Key, Value, Hash, Equal>::cbegin() const {
  return const_iterator(table.data(), table.data() + table.size());
}
template<class Key, class Value, class Hash, class Equal>
typename HashTable<Key, Value, Hash, Equal>::const_iterator HashTable<Key, Value, Hash, Equal>::cend() const {
  return const_iterator(table.data() + table.size(), table.data() + table.size());
}
#endif
