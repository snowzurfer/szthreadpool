
#include <memory>
#include <queue>
#include <mutex>

namespace sz {

template <class T>
class ThreadsafeQueue {
 public:
  ThreadsafeQueue();

  // Pushes a new value into the queue
  void Push(T new_value);
  // Pops a value. Puts thread doing the pop asleep if there aren't any values
  // to be popped. This version puts the value into a variable passed as a
  // reference.
  void WaitAndPop(T &value);
  // Pops a value. Puts thread doing the pop asleep if there aren't any values
  // to be popped. This version returns the value into an std::shared_ptr.
  std::shared_ptr<T> WaitAndPop();
  // Attempts to pop a value into a parameter passed as a reference.
  // Returns false if the queue is emtpy.
  bool TryPop(T &value);
  // Attempts to pop a value into an std::shared_ptr.
  // Returns nullptr if the queue is empty.
  std::shared_ptr<T> TryPop();
  bool IsEmpty() const;

 private:
  mutable std::mutex mut_;
  std::queue<std::shared_ptr<T>> data_queue_;
  std::condition_variable data_cond_;

}; // class ThreadsafeQueue

} // namespace sz