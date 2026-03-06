#define BOOST_TEST_MODULE list_test
#include <boost/test/included/unit_test.hpp>
#include "List.h"
#include <string>
#include <stdexcept>
#include <limits>

using namespace ivanov;

template <typename T>
bool check_list_content(const List<T>& lst, const std::initializer_list<T>& expected) {
  if (lst.size() != expected.size()) return false;
  auto it = lst.cbegin();
  auto exp_it = expected.begin();
  while (it != lst.cend() && exp_it != expected.end()) {
    if (*it != *exp_it) return false;
    ++it;
    ++exp_it;
  }
  return true;
}

BOOST_AUTO_TEST_SUITE(list_tests)

BOOST_AUTO_TEST_CASE(default_constructor) {
  List<int> lst;
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
  BOOST_CHECK(lst.begin() == lst.end());
  BOOST_CHECK(lst.cbegin() == lst.cend());
}

BOOST_AUTO_TEST_CASE(push_front_lvalue) {
  List<int> lst;
  int a = 10, b = 20;
  lst.push_front(a);
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
  BOOST_CHECK_EQUAL(lst.back(), 10);

  lst.push_front(b);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.back(), 10);

  BOOST_CHECK(check_list_content(lst, {20, 10}));
}

BOOST_AUTO_TEST_CASE(push_front_rvalue) {
  List<std::string> lst;
  lst.push_front(std::string("world"));
  lst.push_front(std::string("hello"));
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), "hello");
  BOOST_CHECK_EQUAL(lst.back(), "world");
}

BOOST_AUTO_TEST_CASE(sum_overflow_throws) {
  int a = std::numeric_limits<int>::max();
  BOOST_CHECK_THROW(sum(a, 1), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()