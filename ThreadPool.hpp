#include <string>       
#include <vector>       
#include <atomic>       
#include <thread>      
#include <array>        
#include "SharedQueue.hpp" 
#include "SharedMap.hpp"   

static const int MAXTHREAD=20;    // max number of threads allowed by ThreadPool
static const std::string MONITOR_KILL = "@MONITORKILL";
static const int MONITOR_TIME=1;  // in miliseconds

class ThreadPool {
public:
/* initializes:
 * _url : file name of the first file to work on
 * _th : number of threads to run in parallel
 */  
  void insertJob(const std::string &job, const std::string &thid);
  int  startJobs(void (&jobThread)() );
  void terminateJobs();
  int maxThreads() const {return max_th;};
  void init(const std::string &url, const int &nth) { insertJob(url, "main"); max_th = nth; };
  void getToken() { ++tk_working; };
  void releaseToken() { --tk_working; };
  int numTasksInProgress() const { return tk_working.load(); };
  std::string getJob() { return theQ.get(); };
  bool isJobQueueEmpty() const { return theQ.isEmpty(); };
  std::vector<std::string> getJobsPending() { return theQ.getJobsPending(); };
/* Starts the monitor thread */
  void startMonitor(void (&monThread)() ){ monitorTh = std::thread (monThread); };
/* wait for the monitor to terminate */
  void untilMonitorIsDone() { monitorTh.join(); };

  /* functions for testing only */
  std::vector<std::string> getJobsDone() const { return theM.getMap(); };
  void mapPut(const std::string &job, const std::string &thid) {theM.put(job,thid); };

private:
  SharedQueue theQ;                 // FIFO queue to keep pendings references to work with
  size_t max_th;                    // max number of threads to run
  mutable std::atomic<int> tk_working {0};  // token to indicate how many threads awaken
  SharedMap theM;                   // map to keep exclusive jobs in progress or done
  std::array<std::thread,MAXTHREAD> threads; // list of threds running
  std::thread monitorTh;            // monitor thread
};

/* Main function: execute 
 * 
 * controls the flow of the program. Launches threads, decides when to terminate
 * s: is the address of the date to acquire
 * n: is the number of threads to run in parallel
 * see *.cpp for return conditions (TODO copy paste here later) 
 */
int execute(std::string s, int n);

