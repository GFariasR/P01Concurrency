#include "LRUCache.hpp"
#include <iostream>
#include <chrono>

/************* PERFORMANCE TEST TOOLS  *********************************
/*
*/
typedef std::chrono::high_resolution_clock high_resolution_t;
typedef std::chrono::microseconds microseconds_t;

void test_set_performance(const capacity_t& _cap)
{
  LRUCache lru(_cap);
  high_resolution_t::time_point t1 = high_resolution_t::now();
  // 
  for (capacity_t i = 0; i !=_cap; ++i)
    lru.set(i,i);
  //
  high_resolution_t::time_point t2 = high_resolution_t::now();
  auto duration = std::chrono::duration_cast<microseconds_t>( t2 - t1 ).count();
  std::cout << _cap << "           " << duration << "            " << 1000*duration/_cap << std::endl;
}

void test_repeated_set_performance(const capacity_t& _cap)
{
  // prepare the list with initial keys
  LRUCache lru(_cap+1);
  for (capacity_t i = 0; i !=_cap+1; ++i)
    lru.set(i,i);
  //
  high_resolution_t::time_point t1 = high_resolution_t::now();
  //
  for (capacity_t i = 1; i !=_cap+1; ++i)
    lru.set(i,i);  
  //
  high_resolution_t::time_point t2 = high_resolution_t::now();
  auto duration = std::chrono::duration_cast<microseconds_t>( t2 - t1 ).count();
  std::cout << _cap << "           " << duration << "            " << 1000*duration/_cap << std::endl;
}

void test_existing_get_performance(const capacity_t& _cap)
{
  // prepare the list with initial keys
  LRUCache lru(_cap+1);
  for (capacity_t i = 0; i !=_cap+1; ++i)
    lru.set(i,i);
  //
  high_resolution_t::time_point t1 = high_resolution_t::now();
  //
  for (capacity_t i = 1; i !=_cap+1; ++i)
    val_t tmp = lru.get(i);  
  //
  high_resolution_t::time_point t2 = high_resolution_t::now();
  auto duration = std::chrono::duration_cast<microseconds_t>( t2 - t1 ).count();
  std::cout << _cap << "           " << duration << "            " << 1000*duration/_cap << std::endl;
}

void test_set_out_of_capacity_performance(const capacity_t& _cap)
{
  // prepare the list with initial keys
  LRUCache lru(_cap);
  for (capacity_t i = 0; i !=_cap; ++i)
    lru.set(i,i);
  //
  high_resolution_t::time_point t1 = high_resolution_t::now();
  //
  for (capacity_t i = _cap+1; i !=2*_cap; ++i)
    val_t tmp = lru.get(i);  
  //
  high_resolution_t::time_point t2 = high_resolution_t::now();
  auto duration = std::chrono::duration_cast<microseconds_t>( t2 - t1 ).count();
  std::cout << _cap << "           " << duration << "            " << 1000*duration/_cap << std::endl;
}

  /* run a basic verification first 
   * add _cap keys, to a cache capacity _cap
   * check the oldest, and newest
   * add one more
   * check the correct oldest and newest
   * @PARAM _cap : LRUCache capacity
   * @RETURN : True if verification ok, false otherwise
   **/
bool test_verification(const capacity_t& _cap)
{
  if (_cap == 0) {
    std::cout << "Capacity can't be 0 : " << std::endl;
    return false;
  }

  key_t key_t_cap = static_cast<key_t>(_cap);
  if (key_t_cap < 0) {
    std::cout << "Capacity exceeds the max limit : " << _cap << std::endl;
    return false;
  }
  
  LRUCache lru(_cap);
  for (capacity_t i = 0; i!=_cap; ++i)
    lru.set(i,10*i);
  
  key_t ky = lru.get_newest_key();
  if (ky != key_t_cap-1) {
    std::cout << "Wrong newest key : " << ky << std::endl;
    return false;
  }

  ky = lru.get_oldest_key();
  if (ky != 0) {
    std::cout << "Wrong odlest key : " << ky << std::endl;
    return false;
  }
  
  return true;
}

  /* Performance measurament of:
   * 1 set when no existing
   * 2 set when existing
   * 3 get when existing
   * 4 set out of capacity
   *
   * additionally, it will be used to learn the following tools:
   * 1 Valgraind : memory utilization
   * 2 gcov : coverage
   **/
int main()
{
  
  // run a basic verification first 
  if (!test_verification(2))
    {
      std::cout << "ooops, something is wrong with the implementation" << std::endl;
      return 0;
    }
    std::cout << "verification ok, ..." << std::endl;			 
   
  /*
   * 1 set when no existing
   *
   * create a new LRUCache with size 1000
   * and set from 0 .. to 999 
   * This is: no repeated keys, room for the new one
   * Measure performance, print results
   *
   * Run 4 times for 1K, 10K, 100K, 1M, 10M to compare results
   **/
    
   std::cout << "**************************************" << std::endl;
  
   std::cout << "Measure no-repeated key set"<< std::endl;
   std::cout << "Capacity     Duration [usec]       Duration per set [nsec]" << std::endl;
   test_set_performance(1000);
   test_set_performance(10000);
   test_set_performance(100000);
   /*  test_set_performance(1000000);
   test_set_performance(10000000);
   */
   /* 2 set when existing
   *
   * create a new LRUCache with size 1001
   * and set from 0 .. to 1000
   * This is: keys are going to be repeated, all of them from 0..1000
   * in order to ensure we access a position that is not the newest or 
   * the oldest, we set the second time from 1..1000
   * therefore, 0 is alwasy the oldest, and the newest is not set again
   * Measure performance, print results
   *
   * Run 4 times for 1K, 10K, 100K, 1M, 10M to compare results
   **/
   std::cout << "**************************************" << std::endl;
  
   std::cout << "Measure repeated key set"<< std::endl;
   std::cout << "Capacity     Duration [usec]       Duration per set [nsec]" << std::endl;
   test_repeated_set_performance(1000);
   test_repeated_set_performance(10000);
   test_repeated_set_performance(100000);
   /*  test_repeated_set_performance(1000000);
   test_repeated_set_performance(10000000);
   */
   /* 3 get when existing
   *
   * Same scenario than 2 get no-repeated
   *
   * Run 4 times for 1K, 10K, 100K, 1M, 10M to compare results
   **/
   std::cout << "**************************************" << std::endl;
  
   std::cout << "Measure existing get"<< std::endl;
   std::cout << "Capacity     Duration [usec]       Duration per set [nsec]" << std::endl;
   test_existing_get_performance(1000);
   test_existing_get_performance(10000);
   test_existing_get_performance(100000);
   /*  test_existing_get_performance(1000000);
   test_existing_get_performance(10000000);
   */
   std::cout << "**************************************" << std::endl;
   /* 4 set out of capacity 
   *
   * Same scenario than 2,  but now set no-repeated from 2000 to 299
   *
   * Run 4 times for 1K, 10K, 100K, 1M, 10M to compare results
   **/
   std::cout << "**************************************" << std::endl;
  
   std::cout << "Measure set out of capacity"<< std::endl;
   std::cout << "Capacity     Duration [usec]       Duration per set [nsec]" << std::endl;
   test_set_out_of_capacity_performance(1000);
   test_set_out_of_capacity_performance(10000);
   test_set_out_of_capacity_performance(100000);
   /*   test_set_out_of_capacity_performance(1000000);
   test_set_out_of_capacity_performance(10000000);
   */
   std::cout << "**************************************" << std::endl;
   std::cout << "eso seria" << std::endl;
   return 0;
}
