#include <boost/test/unit_test.hpp>
#include "HashTable.h"
#include <string>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE (HashtableSuite)

using namespace ivanov;

BOOST_AUTO_TEST_CASE(default_construction) {
    HashTable<int, int, std::hash<int>, std::equal_to<int>> ht;
    BOOST_TEST(ht.size() == 0);
    BOOST_TEST(ht.empty());
    BOOST_TEST(ht.capacity() == 16);
}

BOOST_AUTO_TEST_CASE(add_and_has) {
    HashTable<int, std::string, std::hash<int>, std::equal_to<int>> ht;
    ht.add(1, "one");
    ht.add(2, "two");
    BOOST_TEST(ht.size() == 2);
    BOOST_TEST(ht.has(1));
    BOOST_TEST(ht.has(2));
    BOOST_TEST(!ht.has(3));
}

BOOST_AUTO_TEST_CASE(at_access) {
    HashTable<std::string, int, std::hash<std::string>, std::equal_to<std::string>> ht;
    ht.add("apple", 5);
    ht.add("banana", 7);

    BOOST_TEST(ht.at("apple") == 5);
    BOOST_TEST(ht.at("banana") == 7);

    ht.at("apple") = 10;
    BOOST_TEST(ht.at("apple") == 10);

    BOOST_CHECK_THROW(ht.at("cherry"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(drop_and_tombstone) {
    HashTable<int, int, std::hash<int>, std::equal_to<int>> ht;
    ht.add(1, 100);
    ht.add(2, 200);
    size_t cap = ht.capacity();

    int val = ht.drop(1);
    BOOST_TEST(val == 100);
    BOOST_TEST(ht.size() == 1);
    BOOST_TEST(!ht.has(1));
    BOOST_TEST(ht.has(2));

    ht.add(3, 300);
    BOOST_TEST(ht.has(3));
    BOOST_TEST(ht.size() == 2);
    BOOST_TEST(ht.capacity() == cap);
    BOOST_TEST(ht.at(3) == 300);
}

BOOST_AUTO_TEST_CASE(drop_nonexistent) {
    HashTable<int, int, std::hash<int>, std::equal_to<int>> ht;
    BOOST_CHECK_THROW(ht.drop(42), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(rehash_keeps_elements) {
    HashTable<int, int, std::hash<int>, std::equal_to<int>> ht;
    const int num = 5;
    for (int i = 0; i < num; ++i)
        ht.add(i, i * 10);

    size_t old_cap = ht.capacity();
    ht.rehash(32);
    BOOST_TEST(ht.size() == num);
    BOOST_TEST(ht.capacity() >= 32);
    BOOST_TEST(ht.capacity() != old_cap);

    for (int i = 0; i < num; ++i) {
        BOOST_TEST(ht.has(i));
        BOOST_TEST(ht.at(i) == i * 10);
    }
}

BOOST_AUTO_TEST_CASE(overflow_throws) {
    HashTable<int, int, std::hash<int>, std::equal_to<int>> ht;
    // Заполняем все 16 слотов
    for (int i = 0; i < 16; ++i) {
        ht.add(i, i);
    }
    BOOST_TEST(ht.size() == 16);
    BOOST_CHECK_THROW(ht.add(100, 100), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(clear_table) {
    HashTable<char, char, std::hash<char>, std::equal_to<char>> ht;
    ht.add('a', 'A');
    ht.add('b', 'B');
    BOOST_TEST(ht.size() == 2);

    ht.clear();
    BOOST_TEST(ht.size() == 0);
    BOOST_TEST(ht.empty());
    BOOST_TEST(!ht.has('a'));
    BOOST_TEST(ht.capacity() > 0);
}

BOOST_AUTO_TEST_CASE(const_iterator_basic) {
    HashTable<double, int, std::hash<double>, std::equal_to<double>> ht;
    ht.add(1.1, 100);
    ht.add(2.2, 200);

    const auto& cht = ht;
    std::vector<std::pair<double, int>> items;
    for (auto it = cht.cbegin(); it != cht.cend(); ++it) {
        auto p = *it;
        items.emplace_back(p.first, p.second);
    }
    BOOST_TEST(items.size() == 2);
}
