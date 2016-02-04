
#include <atomic>
#include <thread>
#include "threadsafe_queue.h"
#include <vector>
#include "join_threads.h"
#include <tuple>
#include <cstdint>
#include <atomic>
#include <stdio.h>

namespace sz {

typedef void(*JobFunction)(const int32_t worker_id, const void *input);

class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  // Args:
  // - job: function ptr to the function to be executed as job
  // - input: ptr to a struct containing data to be used with the function
  void Submit(JobFunction job, const void *input);

  void Submit(JobFunction job, const void *input, std::atomic<int32_t> *label);

 private:
  typedef std::tuple<JobFunction, const void *> JobTuple;
  std::atomic_bool done_;
  ThreadsafeQueue<JobTuple> work_queue_;
  std::vector<std::thread> threads_;
  JoinThreads joiner_;

  // Executed by the worker threads to acquire jobs
  void WorkerThread();

}; // class ThreadPool

} // namespace sz