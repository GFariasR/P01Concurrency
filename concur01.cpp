#include "ThreadPool.hpp"
#include <iostream>  
#include <fstream>   
#include <sstream>   
#include <thread>    
#include <chrono>
#include <string>
#include <map>

/*************************************
 *                                   *
 *         Support Functions         *
 *                                   *
 ************************************/   

/* convert a thread id into a str */
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
 * Uses thp as the pool of shared resources
 * 
 * actions:
 * Get a file name from the pool
 * Get a token to work
 * Work list:
 *    - read the file (file name)
 *    - insert the references on the pool to continue the job later
 *    - NOTE: only new ref will inserted on the pool, see ThreadPool::InsertNew
 * Releases the token
 *
 **************************************************************
 */
void task_GetAndPush(ThreadPool& thp)
{
  std::string sid = getStr(std::this_thread::get_id());
  while(true) {
    std::string job = thp.getJob();   // will wait until there is a new job
    if (job == MONITOR_KILL) {
      return;
    }
  // now we get a list of new references to download
  // this is the concurrent part. No shared resource here!
    std::vector<std::string> refs = getReferences(job);
  // now, we need to update shared resources again
  // insert the new references on the queue
    for (auto rjob : refs) {
      // insert the job if it is new
      thp.putIfAbsent(rjob, sid);       
      // now a delay to allow other threads to react
      // ... this is a simulation of a job done taking some time
      std::this_thread::sleep_for (std::chrono::milliseconds(1));
    }
    // and check if exit condition... if all tasks done
    thp.areWeDone();
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
int execute(std::string url, size_t nth)
{
  // minimum validation to prevent naive mistakes
  if (url.empty())
    return 1;
  if ((nth < 1) || (nth > MAXTHREAD))
    return 10;
  std::string refurl = getRef(url);
  if (refurl.empty())
    return 2;    // not actual ref to an internal file

  ThreadPool thp(refurl, nth);            // initializes the pool
  thp.runTasks(task_GetAndPush);
  
  // now, print the map for future references (testing mainly...)
  std::map<std::string, int> resmap;
  std::ofstream ofs(refurl+".a");       // constructor, open the file as well
  if (!ofs)  // output file
    return 20;
  ofs << "printing map on execute()" << std::endl;
  std::vector<std::string> res = thp.getJobsDone();
  int count=0;
  for (auto s : res) {
    ofs << "reference #" << ++count << " is: " << s << std::endl;
    auto sid = s.substr(0,15);
    if (!resmap.empty()) {
      auto ptr = resmap.emplace(sid, 1);
      if (!ptr.second) {
	auto qty = (ptr.first)->second;
	qty++;
	(ptr.first)->second = qty;
      }
    } else {
      resmap.emplace(sid, 1);
    }
  }
  ofs << "bye! " << std::endl;
  ofs.close();

  std::cout << "Result" << std::endl;
  for (auto s : resmap) {
    std::cout << s.first << " = " << s.second << std::endl;
  }
  return 0;
}
