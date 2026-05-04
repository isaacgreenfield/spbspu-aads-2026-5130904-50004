#include <boost/test/unit_test.hpp>
#include "credentials.hpp"
#include <sstream>

BOOST_AUTO_TEST_CASE(credentials_test)
{
  std::ostringstream out;
  chadin::out_credentials(out);
  BOOST_TEST(out.str() == "chadin.maxim");
}
