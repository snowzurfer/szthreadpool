
#include "thread_pool.h"


namespace sz {

//ThreadPool::ThreadPool() :
//    done_(),
//    work_queue_(),
//    threads_(),
//    joiner_(threads_) {
//  // Must initialise done_ here instead of in the initialisation list because
//  // MSVC doesn't have ctors for that type
//  done_ = false;
//
//  const size_t thread_count = std::thread::hardware_concurrency();
//  threads_.resize(thread_count);
//  // Start the worker threads and catch exceptions thrown during creation, then
//  // stop and propagate them to the caller
//  try {
//    for (size_t i = 0; i < thread_count; ++i) {
//      int32_t result = pthread_create(&threads_[i], NULL, &ThreadPool::WorkerThread, this, )
//      threads_.push_back(
//        std::thread(&ThreadPool::WorkerThread, this, i));
//    }
//  }
//  catch (...) {
//    done_ = true;
//    throw;
//  }
//}
//
//ThreadPool::~ThreadPool() {
//  done_ = true;
//}
//
//void ThreadPool::WorkerThread(int32_t tid) {
//  while (!done_) {
//    JobTuple job;
//    if (work_queue_.TryPop(job)) {
//      JobFunction task = std::get<0>(job);
//      const void *input = std::get<1>(job);
//      task(tid, input);
//    }
//    else {
//      std::this_thread::yield();
//    }
//  }
//}
//
//void ThreadPool::Submit(JobFunction job, const void *input) {
//  work_queue_.Push(std::make_tuple(job, input));
//}

} // namespace sz