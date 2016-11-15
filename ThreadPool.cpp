#include "ThreadPool.hpp"
#include <thread>    //
#include <chrono>
#include <iostream>

/*************************************
 *                                   *
 *         ThreadPool                *
 *                                   *
 ************************************/   

ThreadPool::ThreadPool(const std::string &url, const size_t &nth)
{
   max_th = nth;                // max threads
   map.emplace(url, "main");    // insert the job on the map
   q.push(url);                 // and the FIFO
   tk_working = 0;              // no tasks in progress
}

/* 
 * Tries to insert on the map.
 * if succesesful, then inserts it on FIFO queue
 * otherwise, the job is ignored, no need to communicate that
 * thid: str conversion of the thread ID calling this attempt to insert a job
 */
void ThreadPool::putIfAbsent(const std::string &job, const std::string &thid)
{
  std::lock_guard<std::mutex> lck(mtx);  // acquire mutex, or wait for it
  auto res = map.emplace(job, thid);     // try to put on the map
  if (res.second) {
    q.push(job);                          // is safe to update the queue
    // ...signal to anyone (...1)  waiting for a queue update
    cond.notify_one();                    
  }
} // release mtx

/* 
 * Get a job, or wait for one.
 * When the job is gotten takes a token
 */
std::string ThreadPool::getJob()
{
  // we need unique functionaly to wait on condition
  std::unique_lock<std::mutex> lck(mtx);
  // wait until cond is notify, and is not empty
  cond.wait(lck, [this]{return !q.empty();} );
  // safe to get data since lck is owned
  std::string s = q.front();
  // safe to remove the data from the queue
  q.pop();
  // to inform thread busy with job in progres
  tk_working++;                              
  return s;                            
} // release mutex

/* 
 * Check for exit condition: empty FIFO AND no thread busy.
 * if so, insert terminate messages
 */
void ThreadPool::areWeDone()
{
  std::lock_guard<std::mutex> lck(mtx); // acquire mutex, or wait for it
  tk_working--;
  if ((q.empty()) and (tk_working == 0)) {
    for (size_t i=0; i<max_th; ++i) {
      q.push(MONITOR_KILL);
    // ...signal to anyone waiting for a queue update
      cond.notify_one();                    
    }
  }
} // releases mutex

/* Starts the N jobthreads
 * job threads are kept on the vector of threads
 * Wait for all the threads to finish
 */
void ThreadPool::runTasks(void (&jobThread)(ThreadPool&) )
{
  // let's rock and roll
  for (size_t i=0; i<max_th; ++i)
    threads[i] = std::thread(jobThread, std::ref(*this));

  std::cout <<  "all threads running (" << max_th << ")"
	    << "... waiting for them to join"
	    << std::endl;
  
  //   wait for the tasks to finish (kill themselves)   
  for (size_t i=0; i<max_th; ++i)
    threads[i].join();
}

/* get the whole map in a vector  */
std::vector<std::string> ThreadPool::getJobsDone() const
{
  std::vector<std::string> res;
  for ( auto it = map.cbegin(); it != map.cend(); ++it ) {
    std::string s = it->second + ":->" + it->first;
    res.push_back(s);
  }
  return res;
}
