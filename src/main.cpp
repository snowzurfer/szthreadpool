
//#include "thread_pool.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <pthread.h>
#include <Windows.h>
extern "C"
{
#include "thread_pool_c.h"
}

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
    fprintf(stderr, "PrintFromInput(): input is NULL");
    return;
  }

  const char *string = (const char *)arg->input_buffer;

  printf("%s\n", string);
}

void PrintHello(const int32_t worker_id, const void *input) {
  printf("Hello!\n");
}

void SleepJob(const int32_t worker_id, const void *input) {
  double TIMEOUT = 5.0;
  time_t start, end;
  double tpassed = 0.0;
  time (&start);
  while (tpassed < TIMEOUT) {
    time (&end);
    tpassed = difftime(end,start);
  }

  printf("Finished sleeping!\n");
}

int main() {
  ThreadPool *thread_pool = ThreadPoolInit(2);
  int32_t concurrency = pthread_getconcurrency();
  char *string = "I am here";
  int32_t size = 10;
  PrintJobArg input_arg = { (uint8_t *)string, size};

  ThreadpoolSubmit(thread_pool, PrintHello, nullptr);
  ThreadpoolSubmit(thread_pool, PrintFromInput, &input_arg);
  ThreadpoolSubmit(thread_pool, PrintHello, nullptr);
  ThreadpoolSubmit(thread_pool, PrintHello, nullptr);
  ThreadpoolSubmit(thread_pool, SleepJob, nullptr);

  printf("Now waiting for jobs to complete...\n");
  ThreadpoolWaitAllJobs(thread_pool);

  ThreadpoolDestroy(thread_pool);

  return 0;
};
