#include <unordered_map>       
#include <string>              
#include <mutex>               
#include <vector>               
/*
 * Shared MAP implementation
 * put to insert a new element into the shared map
 * return
 *  1: succesful insertion
 *  0: not possible to insert, element already in the map
 */

class SharedMap {
 public:
  bool put(const std::string &s, const std::string &thid);
  std::vector<std::string> getMap() const;  // for testing purposes only
 private:
  std::mutex mtx;                // to ensure we take ownership before updating
  std::unordered_map<std::string, std::string> map;     
};
