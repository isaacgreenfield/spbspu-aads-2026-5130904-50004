#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <cstddef>
#include <functional>
#include <vector>

template <class Key, class Value, class Hash, class Equal>
class HashTable {
    struct Slot {
        Key key;
        Value value;
        int state = 0;   // 0 – пусто, 1 – занято, 2 – tombstone
    };

    std::vector<Slot> table;
    size_t count = 0;
    Hash hash;
    Equal equal;

    size_t probe(size_t hash_f, size_t i) const noexcept {
        return (hash_f + i * i) % table.size();
    }

    size_t find_slot(const Key& key) const;
    void rehash_internal(size_t new_cap);

public:
    void add(const Key& key, const Value& value);

    bool has(const Key& key) const;

    Value drop(const Key& key);

    void rehash(size_t new_slots);

    size_t size() const noexcept { return count; }
    size_t capacity() const noexcept { return table.size(); }
    bool empty() const noexcept { return count == 0; }

    void clear() noexcept;

    class iterator {
        Slot* ptr;
        Slot* end;
    public:
        iterator(Slot* p, Slot* e) : ptr(p), end(e) { skip_empty(); }
        std::pair<const Key&, Value&> operator*() const;
        iterator& operator++();
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
    };

    class const_iterator {
        const Slot* ptr;
        const Slot* end;
    public:
        const_iterator(const Slot* p, const Slot* e) : ptr(p), end(e) { skip_empty(); }
        std::pair<const Key&, const Value&> operator*() const;
        const_iterator& operator++();
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
    };

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    HashTable();
    HashTable(const HashTable& other);
    ~HashTable() = default;
};

#endif
