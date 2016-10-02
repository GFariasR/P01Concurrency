#include "ThreadPool.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ThreadPool Test Cases
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( BasicTest01 )
{
  std::string url = "<filename:testSpace/file01.txt>";
  int maxth = 10;
  BOOST_CHECK( execute(url, maxth) == 0);
}

