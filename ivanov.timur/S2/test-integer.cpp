#include <boost/test/unit_test.hpp>
#include "fun.h"

BOOST_AUTO_TEST_SUITE (IntegerSuite)

BOOST_AUTO_TEST_CASE (Integer_ArithmeticOperators) {
  Integer a(10), b(3);
  Integer c(10), d(3);
  BOOST_CHECK_EQUAL((a + b).getValue(), 13);
  BOOST_CHECK_EQUAL((c - d).getValue(), 7);
  BOOST_CHECK_EQUAL((a * b).getValue(), 39);
  BOOST_CHECK_EQUAL((a / b).getValue(), 13);
  BOOST_CHECK_EQUAL((a % b).getValue(), 1);
}

BOOST_AUTO_TEST_CASE (Integer_Concatenation) {
  Integer a(12), b(34);
  a.concatation(b);
  BOOST_CHECK_EQUAL(a.getValue(), 1234);
}

BOOST_AUTO_TEST_CASE (Integer_Gcd) {
  Integer a(48), b(18);
  a.gcd(b);
  BOOST_CHECK_EQUAL(a.getValue(), 6);
}

BOOST_AUTO_TEST_CASE (Integer_Lcm) {
  Integer a(4), b(6);
  a.lcm(b);
  BOOST_CHECK_EQUAL(a.getValue(), 12);
}

BOOST_AUTO_TEST_SUITE_END()
