
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace sz {
//template <class T>
//class ThreadsafeQueue {
// public:
//  ThreadsafeQueue() : mut_(), data_queue_(), data_cond_() {};
//
//  // Pushes a new value into the queue
//  void Push(T new_value) {
//    std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
//    
//    std::lock_guard<std::mutex> lk(mut_);
//    data_queue_.push(data);
//    data_cond_.notify_one();
//  }
//  // Pops a value. Puts thread doing the pop asleep if there aren't any values
//  // to be popped. This version puts the value into a variable passed as a
//  // reference.
//  void WaitAndPop(T &value) {
//    std::lock_guard<std::mutex> lk(mut_);
//    if (data_queue_.empty()) {
//      return false;
//    }
//    value = std::move(*data_queue_.front());
//    data_queue_.pop();
//    return true;
//  }
//
//  // Pops a value. Puts thread doing the pop asleep if there aren't any values
//  // to be popped. This version returns the value into an std::shared_ptr.
//  std::shared_ptr<T> WaitAndPop() {
//    std::unique_lock<std::mutex> lk(mut_);
//    data_cont.wait(lk, [this]{return !data_queue_.empty();});
//    std::shared_ptr<T> value = data_queue_.front();
//    data_queue_.pop();
//    return value;
//  }
//
//  // Attempts to pop a value into a parameter passed as a reference.
//  // Returns false if the queue is emtpy.
//  bool TryPop(T &value) {
//    std::lock_guard<std::mutex> lk(mut_);
//    if (data_queue_.empty()) {
//      return false;
//    }
//    value = std::move(*data_queue_.front());
//    data_queue_.pop();
//    return true;
//  }
//
//  // Attempts to pop a value into an std::shared_ptr.
//  // Returns nullptr if the queue is empty.
//  std::shared_ptr<T> TryPop() {
//    std::lock_guard<std::mutex> lk(mut_);
//    if (data_queue_.empty()) {
//      return std::shared_ptr<T>();
//    }
//    std::shared_ptr<T> value = data_queue_.front();
//    data_queue_.pop();
//    return value;
//  }
//
//  bool IsEmpty() const {
//    std::lock_guard<std::mutex> lk(mut_);
//    return data_queue_.empty();
//  }
//
// private:
//  mutable std::mutex mut_;
//  mutable phtread_mutex_t mutex_;
//  std::queue<std::shared_ptr<T>> data_queue_;
//  std::condition_variable data_cond_;

//}; // class ThreadsafeQueue

} // namespace sz