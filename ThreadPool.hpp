#include <string>       
#include <vector>       
#include <thread>      
#include <array>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>

// max number of threads allowed by ThreadPool
static const int MAXTHREAD=20;    
static const std::string MONITOR_KILL = "@MONITORKILL";

class ThreadPool {
public:
  ThreadPool(const std::string &url, const size_t &nth);

  void runTasks(void (&jobThread)(ThreadPool&));
  // add a job to the list if it does not exist
  void putIfAbsent(const std::string &job, const std::string &thid);
  // get a job from the list to work on, thread busy
  std::string getJob();
  // indicates the job is completed, thread free
  void areWeDone(); 

  // testing purpose only...
  std::vector<std::string> getJobsDone() const;

private:
  size_t max_th;                    // max number of threads to run
  std::array<std::thread,MAXTHREAD> threads; // list of threds running

  std::queue<std::string> q;        // FIFO queue to keep the pending jobs
  // map to keep exclusive jobs in progress or done
  std::unordered_map<std::string, std::string> map;      

  /* to control shared resources access */
  mutable std::mutex mtx;     // to ensure we take ownership before updating
  // to trigger an event when there is a new element
  mutable std::condition_variable cond;
  // token to indicate how many threads are busy (awaken)
  mutable int tk_working;                       
};
