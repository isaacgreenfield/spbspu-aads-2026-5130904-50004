#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char* argv[]);
#endif

#define BOOST_TEST_MODULE ListTests
#include <boost/test/included/unit_test.hpp>
#include "list.hpp"

using namespace ivanov;

BOOST_AUTO_TEST_CASE(push)
{
  List< int > list;
  BOOST_CHECK(list.empty());

  list.push_back(10);
  list.push_front(5);
  BOOST_CHECK_EQUAL(list.front(), 5);
  BOOST_CHECK_EQUAL(list.back(), 10);
}

BOOST_AUTO_TEST_CASE(pop)
{
  List< int > list;
  list.push_back(10);
  list.push_front(5);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.front(), 10);
  list.pop_back();
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(splice_all)
{
    List<int> a, b;
    a.push_back(1);
    a.push_back(2);
    b.push_back(3);
    b.push_back(4);

    auto pos = a.cbegin(); // указывает на 1
    a.splice(pos, b);
    BOOST_CHECK(b.empty());
    BOOST_CHECK_EQUAL(a.size(), 4);
    // порядок: 1, 3, 4, 2 (вставка после 1)
    auto it = a.cbegin();
    BOOST_CHECK_EQUAL(*it++, 1);
    BOOST_CHECK_EQUAL(*it++, 3);
    BOOST_CHECK_EQUAL(*it++, 4);
    BOOST_CHECK_EQUAL(*it++, 2);
}

BOOST_AUTO_TEST_CASE(splice_one)
{
    List<int> a, b;
    a.push_back(10);
    b.push_back(20);
    b.push_back(30);
    auto it_b = b.cbegin(); // указывает на 20
    a.splice(a.cbegin(), b, it_b); // перенести 20 после 10
    BOOST_CHECK_EQUAL(a.size(), 2);
    BOOST_CHECK_EQUAL(b.size(), 1);
    auto ia = a.cbegin();
    BOOST_CHECK_EQUAL(*ia++, 10);
    BOOST_CHECK_EQUAL(*ia, 20);
}

BOOST_AUTO_TEST_CASE(sort_and_merge)
{
    List<int> lst;
    lst.push_back(5);
    lst.push_back(2);
    lst.push_back(8);
    lst.push_back(1);
    lst.sort();
    int prev = 0;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it) {
        BOOST_CHECK(*it >= prev);
        prev = *it;
    }

    List<int> other;
    other.push_back(7);
    other.push_back(3);
    other.sort();
    lst.merge(other);
    BOOST_CHECK(other.empty());
    prev = 0;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it) {
        BOOST_CHECK(*it >= prev);
        prev = *it;
    }
    BOOST_CHECK_EQUAL(lst.size(), 6);
}

BOOST_AUTO_TEST_CASE(partition_test)
{
    List<int> lst;
    for (int x : {1, 2, 3, 4, 5, 6}) lst.push_back(x);
    lst.partition([](int x){ return x % 2 == 0; });
    bool found_odd = false;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it) {
        if (*it % 2 != 0) found_odd = true;
        else BOOST_CHECK(!found_odd);
    }
}
