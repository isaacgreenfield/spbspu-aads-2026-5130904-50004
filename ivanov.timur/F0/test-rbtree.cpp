#include <boost/test/included/unit_test.hpp>
#include "rbtree.h"
#include <string>
#include <vector> //это только для тестов!!!
#include <algorithm>

using Tree = ivanov::RBtree<int, std::string>;

BOOST_AUTO_TEST_SUITE(RBTree)

BOOST_AUTO_TEST_CASE(Empty)
{
  Tree t;
  BOOST_CHECK(t.empty());
  BOOST_CHECK_EQUAL(t.size(), 0);
  BOOST_CHECK(t.begin() == t.end());
  BOOST_CHECK(!t.contains(5));
  BOOST_CHECK(t.find(5) == t.end());
}

BOOST_AUTO_TEST_CASE(InsertSingle)
{
  Tree t;
  t.insert(10, "ten");
  BOOST_CHECK(!t.empty());
  BOOST_CHECK_EQUAL(t.size(), 1);
  BOOST_CHECK(t.contains(10));
  BOOST_CHECK(!t.contains(5));

  auto it = t.find(10);
  BOOST_REQUIRE(it != t.end());
  BOOST_CHECK_EQUAL(it->first, 10);
  BOOST_CHECK_EQUAL(it->second, "ten");
}

BOOST_AUTO_TEST_CASE(InsertDuplicate)
{
  Tree t;
  t.insert(1, "one");
  t.insert(1, "ONE");
  BOOST_CHECK_EQUAL(t.size(), 1);
  auto it = t.find(1);
  BOOST_REQUIRE(it != t.end());
  BOOST_CHECK_EQUAL(it->second, "ONE");
}

BOOST_AUTO_TEST_CASE(InsertManyAndOrder)
{
  Tree t;
  std::vector<int> keys = {5, 3, 7, 1, 9, 4, 6, 2, 8, 0};
  for (int k : keys) {
    t.insert(k, std::to_string(k));
  }

  BOOST_CHECK_EQUAL(t.size(), keys.size());

  std::vector<int> observed;
  for (auto it = t.begin(); it != t.end(); ++it) {
    observed.push_back(it->first);
  }
  std::vector<int> expected = keys;
  std::sort(expected.begin(), expected.end());
  BOOST_CHECK(observed == expected);
}

BOOST_AUTO_TEST_CASE(Find)
{
  Tree t;
  t.insert(100, "hundred");
  t.insert(50, "fifty");
  t.insert(150, "one fifty");

  auto it = t.find(50);
  BOOST_REQUIRE(it != t.end());
  BOOST_CHECK_EQUAL(it->second, "fifty");

  it = t.find(200);
  BOOST_CHECK(it == t.end());
}

BOOST_AUTO_TEST_CASE(Remove)
{
  Tree t;
  t.insert(8, "a");
  t.insert(3, "b");
  t.insert(10, "c");
  t.insert(1, "d");
  t.insert(6, "e");
  t.insert(14, "f");
  t.insert(4, "g");
  t.insert(7, "h");
  t.insert(13, "i");

  t.remove(1);
  BOOST_CHECK(!t.contains(1));
  BOOST_CHECK_EQUAL(t.size(), 8);

  t.remove(14);
  BOOST_CHECK(!t.contains(14));
  BOOST_CHECK(t.contains(13));

  t.remove(3);
  BOOST_CHECK(!t.contains(3));
  BOOST_CHECK(t.contains(4));

  std::vector<int> ordered;
  for (auto it = t.begin(); it != t.end(); ++it) ordered.push_back(it->first);
  BOOST_CHECK(std::is_sorted(ordered.begin(), ordered.end()));
  BOOST_CHECK_EQUAL(t.size(), 6);
}

BOOST_AUTO_TEST_CASE(RemoveNonExistent)
{
  Tree t;
  t.insert(5, "five");
  t.remove(999);
  BOOST_CHECK_EQUAL(t.size(), 1);
  BOOST_CHECK(t.contains(5));
}

BOOST_AUTO_TEST_CASE(Iterator)
{
  Tree t;
  t.insert(2, "b");
  t.insert(1, "a");
  t.insert(3, "c");

  auto it = t.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 2);
  it++;
  BOOST_CHECK_EQUAL(it->first, 3);
  ++it;
  BOOST_CHECK(it == t.end());

  --it;
  BOOST_CHECK_EQUAL(it->first, 3);
  it--;
  BOOST_CHECK_EQUAL(it->first, 2);
  --it;
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK(it == t.begin());
}

BOOST_AUTO_TEST_CASE(ConstIterator)
{
  Tree t;
  t.insert(10, "ten");
  t.insert(20, "twenty");

  const Tree& ct = t;
  auto it = ct.begin();
  BOOST_CHECK(it != ct.end());
  BOOST_CHECK_EQUAL(it->first, 10);

  auto cit = ct.cbegin();
  BOOST_CHECK(cit == ct.begin());
  ++cit;
  BOOST_CHECK_EQUAL(cit->first, 20);
  BOOST_CHECK(ct.cend() == ct.end());
}

BOOST_AUTO_TEST_CASE(ForEach)
{
  Tree t;
  t.insert(3, "three");
  t.insert(1, "one");
  t.insert(2, "two");

  std::vector<int> keys;
  t.forEach([&keys](int k, const std::string&) {
    keys.push_back(k);
  });
  BOOST_REQUIRE_EQUAL(keys.size(), 3);
  BOOST_CHECK(keys == (std::vector<int>{1, 2, 3}));
}

BOOST_AUTO_TEST_CASE(Clear)
{
  Tree t;
  t.insert(1, "a");
  t.insert(2, "b");
  t.clear();

  BOOST_CHECK(t.empty());
  BOOST_CHECK_EQUAL(t.size(), 0);
  BOOST_CHECK(t.begin() == t.end());
  BOOST_CHECK(!t.contains(1));
}

BOOST_AUTO_TEST_CASE(Stress)
{
  Tree t;
  const int N = 1000;
  for (int i = 0; i < N; ++i) {
    t.insert(i, std::to_string(i));
  }
  BOOST_CHECK_EQUAL(t.size(), N);

  for (int i = 0; i < N; i += 2) {
    t.remove(i);
  }
  BOOST_CHECK_EQUAL(t.size(), N / 2);

  for (int i = 1; i < N; i += 2) {
    BOOST_CHECK_MESSAGE(t.contains(i), "Missing key " << i);
  }
  for (int i = 0; i < N; i += 2) {
    BOOST_CHECK_MESSAGE(!t.contains(i), "Unexpected key " << i);
  }

  std::vector<int> v;
  for (auto it = t.begin(); it != t.end(); ++it) v.push_back(it->first);
  BOOST_CHECK(std::is_sorted(v.begin(), v.end()));
}

BOOST_AUTO_TEST_SUITE_END()
