#include "LRUCache.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LCA Test Cases
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( BasicTest01 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  BOOST_CHECK( lru.get(2) == 20);
}

BOOST_AUTO_TEST_CASE( BasicTest02 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  BOOST_CHECK( lru.get(3) == 30);
}

BOOST_AUTO_TEST_CASE( BasicTest03 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  BOOST_CHECK( lru.get(1) == 10);
}

BOOST_AUTO_TEST_CASE( BasicTest04 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  BOOST_CHECK( lru.get(5) == -1);
}

/* now test capacity */
BOOST_AUTO_TEST_CASE( CapTest01 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  lru.set(4,40);
  lru.set(5,50);
  BOOST_CHECK( lru.get(5) == 50);
}

BOOST_AUTO_TEST_CASE( CapTest02 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  lru.set(4,40);    
  BOOST_CHECK( lru.get(1) == 10);  // still in the cache, but now is newest
  lru.set(5,50);     // 2 should be out of the cache
  BOOST_CHECK( lru.get(2) == -1);
  BOOST_CHECK( lru.get(5) == 50); // and 5 in
}

BOOST_AUTO_TEST_CASE( CapTest03 )
{
  LRUCache lru(4);   // class to be tested
  lru.set(1,10);
  lru.set(2,20);
  lru.set(3,30);
  lru.set(4,40);    
  BOOST_CHECK( lru.get(1) == 10);  // still in the cache, but now is newest
  lru.set(5,50);     // 2 should be out of the cache
  BOOST_CHECK( lru.get(2) == -1);
  BOOST_CHECK( lru.get(5) == 50); // and 5 in, and still newest
  lru.set(3,30);    // 3 is now newest, 4 is oldest
  lru.set(6,60);    // 4 is out, 6 is the newest  
  BOOST_CHECK( lru.get(4) == -1);
  BOOST_CHECK( lru.get(6) == 60); // and 6 in
}

/*
2,[set(2,1),set(2,2),get(2),set(1,1),set(4,1),get(2)]
out:: [2,-1]
*/
BOOST_AUTO_TEST_CASE( LeetCode01 )
{
  LRUCache lru(2);   // class to be tested
  lru.set(2,1);
  lru.set(2,2);
  BOOST_CHECK( lru.get(2) == 2);  
  lru.set(1,1);
  lru.set(4,1);
  BOOST_CHECK( lru.get(2) == -1);
}
