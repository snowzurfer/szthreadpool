
#include "threadsafe_queue.h"

namespace sz {

template <class T>
ThreadsafeQueue<T>::ThreadsafeQueue() : mut_(), data_queue_(), data_cond_() {}

template <class T>
void ThreadsafeQueue<T>::Push(T new_value) {
  std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
  std::lock_guard<std::mutex> lk(mut_);
  data_queue_.push(data);
  data_cond.notify_one();
}

template <class T>
void ThreadsafeQueue<T>::WaitAndPop(T &value) {
  std::unique_lock<std::mutex> lk(mut_);
  data_cont.wait(lk, [this]{return !data_queue_.empty();});
  value = std::move(*data_queue_.front());
  data_queue_.pop();
}

template <class T>
std::shared_ptr<T> ThreadsafeQueue<T>::WaitAndPop() {
  std::unique_lock<std::mutex> lk(mut_);
  data_cont.wait(lk, [this]{return !data_queue_.empty();});
  std::shared_ptr<T> value = data_queue_.front();
  data_queue_.pop();
  return value;
}

template <class T>
bool ThreadsafeQueue<T>::TryPop(T &value) {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return false;
  }
  value = std::move(*data_queue_.front());
  data_queue_.pop();
  return true;
}

template <class T>
std::shared_ptr<T> ThreadsafeQueue<T>::TryPop() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return std::shared_ptr<T>();
  }
  std::shared_ptr<T> value = data_queue_.front();
  data_queue_.pop();
  return value;
}

} // namespace sz