#include <boost/test/unit_test.hpp>
#include <sstream>
#include "output.hpp"

BOOST_AUTO_TEST_CASE(output_test)
{
  std::ostringstream out;
  khairullin::output(out);
  BOOST_TEST(out.str() == "khairullin.vadim");
}
