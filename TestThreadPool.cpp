#include "concur01.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ThreadPool Test Cases
#include <boost/test/unit_test.hpp>

// FailTests 
BOOST_AUTO_TEST_CASE( NoUrl )
{
  std::string url = "";
  size_t maxth = 2;
  BOOST_CHECK( execute(url, maxth) == 1);
}

BOOST_AUTO_TEST_CASE( NumThreadLow )
{
  std::string url = "ThisNameDoesNotMatter";
  size_t maxth = 0;
  BOOST_CHECK( execute(url, maxth) == 10);
}

BOOST_AUTO_TEST_CASE( NumThreadHigh )
{
  std::string url = "ThisNameDoesNotMatter";
  size_t maxth = 25;
  BOOST_CHECK( execute(url, maxth) == 10);
}

// Test calling the ThreadPool 
BOOST_AUTO_TEST_CASE( NonExistingFile )
{
  std::string url = "<filename:testSpace/ThisFileDoesNotExist>";
  size_t maxth = 2;
  BOOST_CHECK( execute(url, maxth) == 0);
}

// Test01: file with no references 
BOOST_AUTO_TEST_CASE( Test01NumThreads2 )
{
  std::string url = "<filename:testSpace/test01>";
  size_t maxth = 1;
  BOOST_CHECK( execute(url, maxth) == 0);
}

// Test02: file with one reference, to Test01 
BOOST_AUTO_TEST_CASE( Test02NumThreads2 )
{
  std::string url = "<filename:testSpace/test02>";
  size_t maxth = 2;
  BOOST_CHECK( execute(url, maxth) == 0);
}

// File01: file with multiple references, to files with multiple as well
BOOST_AUTO_TEST_CASE( TestFile01NumThread2 )
{
  std::string url = "<filename:testSpace/file01.txt>";
  size_t maxth = 2;
  BOOST_CHECK( execute(url, maxth) == 0);
}
