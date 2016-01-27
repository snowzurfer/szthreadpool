
#include <thread>
#include <vector>

namespace sz {
class JoinThreads {
 public:
  explicit JoinThreads(std::vector<std::thread> &threads) :
    threads_(threads) {}

  ~JoinThreads() {
    for(size_t i = 0; i < threads_.size(); ++i) {
      if(threads_[i].joinable()) {
        threads_[i].join();
      }
    }
  }

 private:
  std::vector<std::thread> &threads_;

}; // class JointThreads

} // namespace sz
