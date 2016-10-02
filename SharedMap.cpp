#include "SharedMap.hpp"
/* insert a new element on the map
 * return 0 if succesful
 * return 1 if the element already existed
 */
bool SharedMap::put(const std::string &s, const std::string &thid)
{
  std::lock_guard<std::mutex> lck(mtx); // acquire mutex, or wait for it
  auto res = map.emplace(s, thid);      // now is safe to update the map
  return res.second;                    // true if inserted
} // implicitly releases the lck, therefore the mutex

std::vector<std::string> SharedMap::getMap() const  // for testing purposes only
{
  // get the whole map on a vector
  std::vector<std::string> res;
  
  for ( auto it = map.begin(); it != map.end(); ++it ) {
    std::string s = it->second + ":->" + it->first;
    res.push_back(s);
  }
  return res;
}
