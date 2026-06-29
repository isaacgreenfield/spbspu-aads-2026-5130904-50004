#include <boost/test/unit_test.hpp>
#include "BSTtree.h"
#include <string>
#include <stdexcept>
#include <functional>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_SUITE(BSTtreeSuite)

BOOST_AUTO_TEST_CASE(default_construction)
{
    BSTtree<int, std::string, std::less<int>> tree;
    BOOST_TEST(tree.height() == 0);
    // сравниваем итераторы, но не требуем их печати
    BOOST_TEST(!!(tree.begin() == tree.end()));
    BOOST_TEST(!!(tree.cbegin() == tree.cend()));
    BOOST_CHECK_THROW(tree.get(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(push_and_get)
{
    BSTtree<int, std::string, std::less<int>> tree;
    tree.push(1, "one");
    tree.push(2, "two");
    tree.push(0, "zero");

    BOOST_TEST(tree.get(0) == "zero");
    BOOST_TEST(tree.get(1) == "one");
    BOOST_TEST(tree.get(2) == "two");
}

BOOST_AUTO_TEST_CASE(push_duplicate_key_updates_value)
{
    BSTtree<int, std::string, std::less<int>> tree;
    tree.push(10, "ten");
    tree.push(10, "TEN");
    BOOST_TEST(tree.get(10) == "TEN");
}

BOOST_AUTO_TEST_CASE(get_missing_key_throws)
{
    BSTtree<int, double, std::less<int>> tree;
    tree.push(5, 3.14);
    BOOST_CHECK_THROW(tree.get(4), std::out_of_range);
    BOOST_CHECK_THROW(tree.get(6), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(drop_existing_key)
{
    BSTtree<char, int, std::less<char>> tree;
    tree.push('a', 100);
    tree.push('b', 200);
    tree.push('c', 300);

    int removed = tree.drop('b');
    BOOST_TEST(removed == 200);

    BOOST_CHECK_THROW(tree.get('b'), std::out_of_range);
    BOOST_TEST(tree.get('a') == 100);
    BOOST_TEST(tree.get('c') == 300);
}

BOOST_AUTO_TEST_CASE(drop_missing_key_throws)
{
    BSTtree<int, int, std::less<int>> tree;
    tree.push(1, 10);
    BOOST_CHECK_THROW(tree.drop(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(iterator_traversal)
{
    BSTtree<int, std::string, std::less<int>> tree;
    tree.push(3, "three");
    tree.push(1, "one");
    tree.push(2, "two");
    tree.push(4, "four");

    std::vector<std::pair<int, std::string>> items;
    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
        auto p = *it;
        items.emplace_back(p.first, p.second);
    }

    BOOST_REQUIRE(items.size() == 4);
    BOOST_TEST(items[0].first == 1);
    BOOST_TEST(items[0].second == "one");
    BOOST_TEST(items[1].first == 2);
    BOOST_TEST(items[1].second == "two");
    BOOST_TEST(items[2].first == 3);
    BOOST_TEST(items[2].second == "three");
    BOOST_TEST(items[3].first == 4);
    BOOST_TEST(items[3].second == "four");
}

BOOST_AUTO_TEST_CASE(const_iterator_and_conversion)
{
    BSTtree<std::string, double, std::less<std::string>> tree;
    tree.push("pi", 3.14);
    tree.push("e", 2.71);

    auto it = tree.begin();
    auto p = *it;
    BOOST_TEST(p.first == "e");
    BOOST_TEST(p.second == 2.71);

    const auto& ctree = tree;
    std::vector<std::string> keys;
    for (auto ci = ctree.cbegin(); ci != ctree.cend(); ++ci)
        keys.push_back((*ci).first);
    BOOST_REQUIRE(keys.size() == 2);
    BOOST_TEST(keys[0] == "e");
    BOOST_TEST(keys[1] == "pi");
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    BSTtree<int, int, std::less<int>> tree;
    tree.push(10, 100);
    tree.push(20, 200);
    tree.push(30, 300);

    BSTtree<int, int, std::less<int>> moved(std::move(tree));

    BOOST_TEST(moved.get(10) == 100);
    BOOST_TEST(moved.get(20) == 200);
    BOOST_TEST(moved.get(30) == 300);

    BOOST_TEST(!!(tree.begin() == tree.end()));
    BOOST_TEST(tree.height() == 0);
}

BOOST_AUTO_TEST_CASE(rotate_left_keeps_order)
{
    BSTtree<int, char, std::less<int>> tree;
    tree.push(2, 'b');
    tree.push(5, 'e');

    auto it5 = ++tree.cbegin();
    BOOST_REQUIRE(it5 != tree.cend());
    tree.rotateLeft(it5);

    std::vector<int> keys;
    for (auto ci = tree.cbegin(); ci != tree.cend(); ++ci)
        keys.push_back((*ci).first);
    BOOST_REQUIRE(keys.size() == 2);
    BOOST_TEST(keys[0] == 2);
    BOOST_TEST(keys[1] == 5);
}

BOOST_AUTO_TEST_SUITE_END()
