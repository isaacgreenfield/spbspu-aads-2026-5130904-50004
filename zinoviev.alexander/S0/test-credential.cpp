#define BOOST_TEST_MODULE S0
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include "credential.hpp"

BOOST_AUTO_TEST_CASE(credential_test)
{
  std::ostringstream out;
  zinoviev::out_credential(out);
  BOOST_TEST(out.str() == "zinoviev.alexander");
}
