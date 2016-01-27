
#include "thread_pool.h"
#include <iostream>
#include <thread>

void PrintStuff(const int32_t worker_id, const void *input) {
  std::cout << "Hello" << std::endl;
}

int main() {
  sz::ThreadPool thread_pool;
  thread_pool.Submit(PrintStuff, nullptr);
  thread_pool.Submit(PrintStuff, nullptr);
  thread_pool.Submit(PrintStuff, nullptr);

  char input = 'A';
  while (input != 'C') {
    input = std::getchar();
  }

  return 0;
};
