
#include "thread_pool.h"
#include <iostream>
#include <thread>
#include <atomic>

typedef struct PrintJobArg {
  uint8_t *input_buffer;
  int32_t input_buffer_size;
} PrintJobArg;

std::atomic<int32_t> done_label = 0;

void PrintFromInput(const int32_t worker_id, const void *input) {
  if (input == nullptr) {
    return;
  }

  const PrintJobArg *arg = (const PrintJobArg *)input;

  if (arg->input_buffer == nullptr) {
    return;
  }

  const char *string = (const char *)arg->input_buffer;
  
  std::cout << string << std::endl;
}

void PrintHello(const int32_t worker_id, const void *input) {
  std::cout << "Hello" << std::endl;
}

int main() {
  sz::ThreadPool thread_pool;
  char string[] = "I am here\n";
  int32_t size = 11;
  PrintJobArg input_arg = { (uint8_t *)&string, size};

  thread_pool.Submit(PrintHello, nullptr);
  thread_pool.Submit(PrintFromInput, &input_arg);
  thread_pool.Submit(PrintHello, nullptr);

  char input = 'A';
  while (input != 'C') {
    input = std::getchar();
  }

  return 0;
};
