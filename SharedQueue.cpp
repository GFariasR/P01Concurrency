#include "SharedQueue.hpp"

void SharedQueue::put(const std::string &s)
{
  std::lock_guard<std::mutex> lck(mtx);      // acquire mutex, or wait for it
  q.push(s);                            // now is safe to update the queue
  cond.notify_one();                    // to trigger anyone waiting for an queue update
} // implicitly releases the lck, therefore the mutex

std::string SharedQueue::get()
{
  std::unique_lock<std::mutex> lck(mtx);     // we need unique functionaly to wait on condition
  cond.wait(lck, [this]{return !q.empty();} );  // wait until cond is notify, and is not empty
  std::string s = q.front();                 // safe to get data since lck is owned
  q.pop();                                   // safe to remove the data from the queue
  return s;                            
} // implicitly releases the lck, therefore the mutex

bool SharedQueue::isEmpty() const 
{
  std::lock_guard<std::mutex> lck(mtx);      // acquire mutex, or wait for it
  return q.empty();
} // releases mutex implicitly

/* to get the jobs on the FIFO , testing only 
 * in addition, it will empty the FIFO
 */
std::vector<std::string> SharedQueue::getJobsPending()
{
  std::vector<std::string> res;
  while (!isEmpty())
      res.push_back(get());
  return res;
}
