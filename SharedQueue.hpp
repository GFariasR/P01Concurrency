#include <queue>                
#include <string>               
#include <mutex>              
#include <condition_variable>   
#include <vector>              

/*
 * Shared FIFO implementation
 * put to insert a new element into the shared queue
 * get to get and remove the oldest element from the shared queue
 *     a thread that pops will sleep unless there is a value on the queue 
 */

class SharedQueue {
 public:
  void put(const std::string &s);
  std::string get();
  bool isEmpty() const;
  std::vector<std::string> getJobsPending();   // for testing purpose only
 private:
  mutable std::mutex mtx;                // to ensure we take ownership before updating
  mutable std::condition_variable cond;  // to trigger an event when there is a new element
  std::queue<std::string> q;     // FIFO queue
};
