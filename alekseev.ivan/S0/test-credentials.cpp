#define BOOST_TEST_MODULE S0
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include "credentials.h"

BOOST_AUTO_TEST_CASE(credentials_test)
{
  std::ostringstream out;
  alekseev::out_credentials(out);
  BOOST_TEST(out.str() == "alekseev.ivan");
}
