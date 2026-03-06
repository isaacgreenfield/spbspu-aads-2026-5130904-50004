#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
#define BOOST_TEST_MODULE projectTests
#include <boost/test/included/unit_test.hpp>
#include "List.h"

using namespace ivanov;

BOOST_AUTO_TEST_SUITE(ListTests)

BOOST_AUTO_TEST_CASE(DefaultConstructorAndCapacity) {
    List<int> lst;
    BOOST_CHECK(lst.empty());
    BOOST_CHECK_EQUAL(lst.size(), 0);
}

BOOST_AUTO_TEST_CASE(PushAndPopFront) {
    List<int> lst;
    lst.push_front(10);
    BOOST_CHECK(!lst.empty());
    BOOST_CHECK_EQUAL(lst.size(), 1);
    BOOST_CHECK_EQUAL(lst.front(), 10);
    BOOST_CHECK_EQUAL(lst.back(), 10);

    lst.push_front(20);
    BOOST_CHECK_EQUAL(lst.size(), 2);
    BOOST_CHECK_EQUAL(lst.front(), 20);
    BOOST_CHECK_EQUAL(lst.back(), 10);

    lst.pop_front();
    BOOST_CHECK_EQUAL(lst.size(), 1);
    BOOST_CHECK_EQUAL(lst.front(), 10);

    lst.pop_front();
    BOOST_CHECK(lst.empty());

    lst.pop_front();
    BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(PushAndPopBack) {
    List<int> lst;
    lst.push_back(10);
    BOOST_CHECK(!lst.empty());
    BOOST_CHECK_EQUAL(lst.size(), 1);
    BOOST_CHECK_EQUAL(lst.front(), 10);
    BOOST_CHECK_EQUAL(lst.back(), 10);

    lst.push_back(20);
    BOOST_CHECK_EQUAL(lst.size(), 2);
    BOOST_CHECK_EQUAL(lst.front(), 10);
    BOOST_CHECK_EQUAL(lst.back(), 20);

    lst.pop_back();
    BOOST_CHECK_EQUAL(lst.size(), 1);
    BOOST_CHECK_EQUAL(lst.back(), 10);

    lst.pop_back();
    BOOST_CHECK(lst.empty());

    lst.pop_back();
    BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_SUITE_END()
