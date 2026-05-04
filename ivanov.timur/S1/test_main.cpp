#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char* argv[]);
#endif

#define BOOST_TEST_MODULE ListTests
#include <boost/test/included/unit_test.hpp>
#include "List.h"

using namespace ivanov;

BOOST_AUTO_TEST_CASE(push)
{
  List<int> list;
  BOOST_CHECK(list.empty());

  list.push_back(10);
  list.push_front(5);
  BOOST_CHECK_EQUAL(list.front(), 5);
  BOOST_CHECK_EQUAL(list.back(), 10);
}

BOOST_AUTO_TEST_CASE(pop)
{
  List<int> list;
  list.push_back(10);
  list.push_front(5);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.front(), 10);
  list.pop_back();
  BOOST_CHECK(list.empty());
}
