#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char* argv[]);
#endif

#define BOOST_TEST_MODULE IvanovTests
#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include "Queue.h"
#include "Stack.h"
#include "fun.h"
void cleanupObjectList(ivanov::List<Object*>& lst) {
    while (!lst.empty()) {
        delete lst.front();
        lst.pop_front();
    }
}
//для очереди
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

// тесты стека
BOOST_AUTO_TEST_SUITE(StackSuite)

BOOST_AUTO_TEST_CASE(Stack_IsEmptyInitially) {
    ivanov::List<int> list;
    ivanov::Stack<int> stack(&list);
    BOOST_CHECK(stack.isEmpty());
}

BOOST_AUTO_TEST_CASE(Stack_PushAndDrop) {
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

BOOST_AUTO_TEST_CASE(Stack_Top) {
    ivanov::List<int> list;
    ivanov::Stack<int> stack(&list);
    stack.push(100);
    stack.push(200);
    BOOST_CHECK_EQUAL(stack.top(), 200);
    stack.drop();
    BOOST_CHECK_EQUAL(stack.top(), 100);
}

BOOST_AUTO_TEST_CASE(Stack_DropEmptyThrows) {
    ivanov::List<int> list;
    ivanov::Stack<int> stack(&list);
    BOOST_CHECK_THROW(stack.drop(), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()

// тесты класса ЧИСЛО
BOOST_AUTO_TEST_SUITE(IntegerSuite)

BOOST_AUTO_TEST_CASE(Integer_ArithmeticOperators) {
    Integer a(10), b(3);
    Integer c(10), d(3);
    BOOST_CHECK_EQUAL((a + b).getValue(), 13);
    BOOST_CHECK_EQUAL((c - d).getValue(), 7);
    BOOST_CHECK_EQUAL((a * b).getValue(), 30);
    BOOST_CHECK_EQUAL((a / b).getValue(), 3);
    BOOST_CHECK_EQUAL((a % b).getValue(), 1);
}

BOOST_AUTO_TEST_CASE(Integer_Concatenation) {
    Integer a(12), b(34);
    a.concatation(b);
    BOOST_CHECK_EQUAL(a.getValue(), 154);
}

BOOST_AUTO_TEST_CASE(Integer_Gcd) {
    Integer a(48), b(18);
    a.gcd(b);
    BOOST_CHECK_EQUAL(a.getValue(), 6);
}

BOOST_AUTO_TEST_CASE(Integer_Lcm) {
    Integer a(4), b(6);
    a.lcm(b);
    BOOST_CHECK_EQUAL(a.getValue(), 12);
}

BOOST_AUTO_TEST_SUITE_END()
