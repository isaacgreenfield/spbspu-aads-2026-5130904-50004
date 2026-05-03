#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include "Queue.h"

BOOST_AUTO_TEST_SUITE(QueueSuite)

BOOST_AUTO_TEST_CASE(Queue_IsEmptyInitially) {
  ivanov::List<int> list;
  ivanov::Queue<int> queue(&list);
  BOOST_CHECK(queue.isEmpty());
}

BOOST_AUTO_TEST_CASE(Queue_PushAndDrop) {
  ivanov::List<int> list;
  ivanov::Queue<int> queue(&list);
  queue.push(10);
  queue.push(20);
  queue.push(30);
  BOOST_CHECK(!queue.isEmpty());
  BOOST_CHECK_EQUAL(queue.drop(), 10);
  BOOST_CHECK_EQUAL(queue.drop(), 20);
  BOOST_CHECK_EQUAL(queue.drop(), 30);
  BOOST_CHECK(queue.isEmpty());
}

BOOST_AUTO_TEST_CASE(Queue_DropEmptyThrows) {
  ivanov::List<int> list;
  ivanov::Queue<int> queue(&list);
  BOOST_CHECK_THROW(queue.drop(), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()
