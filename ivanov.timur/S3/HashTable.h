#ifndef HASHTABLE_H
#define HASHTABLE_H

template <class Key, class Value, class Hash, class Equal>
class HashTable {


public:
  void add(Key k, Value v);
  Value drop(Key k);
  bool has(Key k);
  void rehash(size_t slots);

  size_t remaining();
  bool is_full();
  };

#endif
