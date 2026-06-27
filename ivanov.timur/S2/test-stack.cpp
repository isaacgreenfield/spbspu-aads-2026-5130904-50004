#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include "Stack.h"

BOOST_AUTO_TEST_SUITE (StackSuite)

BOOST_AUTO_TEST_CASE (Stack_IsEmptyInitially) {
  ivanov::List<int> list;
  ivanov::Stack<int> stack(&list);
  BOOST_CHECK(stack.isEmpty());
}

BOOST_AUTO_TEST_CASE (Stack_PushAndDrop) {
  ivanov::List<int> list;
  ivanov::Stack<int> stack(&list);
  stack.push(10);
  stack.push(20);
  stack.push(30);
  BOOST_CHECK(!stack.isEmpty());
  BOOST_CHECK_EQUAL(stack.drop(), 30);
  BOOST_CHECK_EQUAL(stack.drop(), 20);
  BOOST_CHECK_EQUAL(stack.drop(), 10);
  BOOST_CHECK(stack.isEmpty());
}

BOOST_AUTO_TEST_CASE (Stack_Top) {
  ivanov::List<int> list;
  ivanov::Stack<int> stack(&list);
  stack.push(100);
  stack.push(200);
  BOOST_CHECK_EQUAL(stack.top(), 200);
  stack.drop();
  BOOST_CHECK_EQUAL(stack.top(), 100);
}

BOOST_AUTO_TEST_CASE (Stack_DropEmptyThrows) {
  ivanov::List<int> list;
  ivanov::Stack<int> stack(&list);
  BOOST_CHECK_THROW(stack.drop(), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()
