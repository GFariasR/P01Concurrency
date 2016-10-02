#include "ThreadPool.hpp"
#include <iostream>  // just for the moment, to be deleted
#include <fstream>   // std::ifstream
#include <sstream>   // std::istringstream
#include <thread>    //
#include <chrono>    

ThreadPool thp;      // control shared resources

/*************************************
 *                                   *
 *         ThreadPool                *
 *                                   *
 ************************************/   

/* insert tries to insert on the map
 * if succesesful, then inserts in the FIFO queue
 * otherwise, the job is ignored, no need to communicate that
 */
void ThreadPool::insertJob(const std::string &job, const std::string &thid)
{
  if (theM.put(job, thid))     // try to put on the map
      thp.theQ.put(job); // if successful, then put in the FIFO queue 
}

/* to terminate the jobs intersting the killing message */
void ThreadPool::terminateJobs()
{
  // first sends the kill signal
  for (size_t i=0; i<max_th; ++i)
    thp.theQ.put(MONITOR_KILL);  
  // then wait for the tasks to finish (kill themselves)
  for (size_t i=0; i<max_th; ++i)
    threads[i].join();
  // everything seems to be fine.. let's go home Dorothy
}

/* Starts the N jobthreads
 * job threads are kept on the vector of threads
 */
int ThreadPool::startJobs(void (&jobThread)() )
{
  // let's rock and roll
  for (size_t i=0; i<max_th; ++i)
    threads[i] = std::thread(jobThread);
}

/*************************************
 *                                   *
 *         Support Functions         *
 *                                   *
 ************************************/   

std::string getStr(const std::thread::id &thid)
{
  std::stringstream ss;
  ss << thid;
  return ss.str();
}

/**************************************************************
 * 
 * getRef(word) checks the string to verify if a ref to a 
 * internal file. 
 * return the file name : testSpace/filename if succesful
 * or empty string otherwise (including external references)
 *
 */
std::string getRef(const std::string &wr)
{
  std::string res="";
  if (wr.size() <= 22)
    return res;        // cannot contain a file name
  if ((wr[0] != '<') || (wr[wr.size()-1] != '>') )
      return res;      // it is not a reference <ref>
  std::string ss = wr.substr(1,9);
  if (ss != "filename:")
    return res;        // does not contain the string filename: at the beginning
  ss = wr.substr(10,10);
  if (ss != "testSpace/")
    return res;        // does not contain the string with the internal folder
// it is a candidate to be an actual correct file name, enough for this project
  return (wr.substr(10,wr.size()-11));      
}

/**************************************************************
 * 
 * Get all the references to internal files from the file fn
 * If succesfull return a list with all the references
 * If any error / or no references on the file return an empty list
 ************************************************************** 
 */
std::vector<std::string> getReferences(std::string fn)
{
  std::vector<std::string> res;      // result with all the references found
  
  // open a file
  std::ifstream ifs(fn);       // constructor, open the file as well
  if (!ifs)  // file does not exist. return empty list
    return res;
  
  // get all lines
  std::string line;
  while (getline(ifs, line)) {
    std::istringstream record(line);
    std::string word;
    while (record >> word) {
      // got a word from the file. Check if it is a reference
      std::string aref = getRef(word);
      if (!aref.empty())
	res.push_back(aref);     // we got a candidate
    }
  }
  
  return res;        // with all the references found
}

/**************************************************************
 *             WORKER THREAD
 * Task to be executed as concurrent threads
 * Get a file name from the pool
 * Get a token to work
 * Work list:
 *    - read the file (file name)
 *    - insert the references on the pool to continue the job later
 *    - NOTE: only new ref will inserted on the pool, see ThreadPool::InsertNew
 * Releases the token
 **************************************************************
 */
void task_GetAndPush()
{
  // insert saludation in the map for testing purposes
  std::string sid = getStr(std::this_thread::get_id());
  std::string s = "SALUDATION" + sid;
  thp.mapPut(s, sid );
  
  while(true) {
    thp.getToken();   // signal that is going to getJob, taken a token
  // intend to get a new file name to read
    std::string fn = thp.getJob();   // if not will wait until there is a new job
    // or we got the kill_your_self signal -> job = MONITOR_KILL
    //
    // Right here: Race condition -> it has job, FIFO is empty, Token still taken
    //          => Monitor will believe we are done, a MONITOR_KILL (for all threads)
    //             will be push.
    //         Review the desing with Cristian
    //
    if (fn == MONITOR_KILL)
      return;
    thp.releaseToken();      // got a job
  // now we get a list of new references to download
  // this is the concurrent part. No shared resource here!
    std::vector<std::string> refs = getReferences(fn);
  // now, we need to update shared resources again
  // insert the new references on the queue
    for (auto rjob : refs) {
      thp.insertJob(rjob, getStr(std::this_thread::get_id() ) );  // insert the job if it is new
      // now a delay to allow other threads to react
      std::this_thread::sleep_for (std::chrono::milliseconds(MONITOR_TIME));      
    }
  }
}

/**************************************************************
 *             MONITOR THREAD
 * Task to be executed as concurrent threads
 * Monitors the condition to terminate the program
 * condition =  no more jobs (FIFO queue empty) 
 *            && all threads waiting for a job (TOKEN == max thread requested)
 * if condition is true, put N jobs = MONITOR_KILL o terminate them
 * joins the threads to confirm everything is clear, and goes home
 * if condition is not true, sleep for MONITOR_TIME (mili seconds)
 * 
 **************************************************************
 */
void task_monitor()
{
  while (true) {
    std::this_thread::sleep_for (std::chrono::milliseconds(MONITOR_TIME));
    // let's check the condition
    if ( (thp.numTasksInProgress() == thp.maxThreads() ) &&
	 (thp.isJobQueueEmpty() ) ) {
      // prepare to go home
      thp.terminateJobs();
      return;
    }
  }
}


/**************************************************************
 * 
/* This is the function that controls the flow of the program
 * launches N threads (nth) to acquire the information concurrently
 * starting for file url
 * url has to have the following structure (not validated yet since, see NOTE) 
 * and decides when the job is completed
 * Return:
 *   0: sucesfull termination of the job
 *   1: url not valid. Does not fullfit the basis structure (? needs more)
 *   2: url is not actual ref to an internal file 
 *  10: nth not valid. Tipically out of range (1..MAXTHREAD)
 *  20: no output file possible to report result
 * -> document errors values and conditions here
 *
 * NOTE: I decided to spend my time learning about concurrency and postpone 
 * the string handling for the next project 
 *
 ************************************************************** 
 */
int execute(std::string url, int nth)
{
  // minimum validation to prevent naive mistakes
  if (url.empty())
    return 1;
  if ((nth < 1) || (nth > MAXTHREAD))
    return 10;
  std::string refurl = getRef(url);
  if (refurl.empty())
    return 2;    // not actual ref to an internal file

  thp.init(refurl, nth);   // initializes the pool
  thp.startJobs(task_GetAndPush);
  thp.startMonitor(task_monitor);

  // now we wait for the monitor to shutdown the application
  thp.untilMonitorIsDone();     
  
  // now, print the map for future references (testing mainly...)
  // output : threadPool.a
  std::ofstream ofs("sal.a");       // constructor, open the file as well
  if (!ofs)  // output file
    return 20;
 
  ofs << "printing map on execute()" << std::endl;

  std::vector<std::string> res = thp.getJobsDone();
  int count=0;
  for (auto s : res)
    ofs << "reference #" << ++count << " is: " << s << std::endl;
  
  ofs << "printing pending jobin FIFO  on execute()" << std::endl;

  // now let's print the pending jobs on the queue
  std::vector<std::string> resp = thp.getJobsPending();
  int countp=0;
  for (auto s : resp)
    ofs << "reference job pending #" << ++countp << " is: " << s << std::endl;
  
  ofs << "how many tasks were executed: " << thp.numTasksInProgress() << std::endl;
  ofs << "bye! " << std::endl;
  ofs.close();

  return 0;
}
