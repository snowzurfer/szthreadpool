
//#include "thread_pool.h"
#include <pthread.h>
#include "thread_pool_c.h"
#include "dbg.h"
#include <stdint.h>
#include <stdio.h>

typedef struct PrintJobArg {
  uint8_t *input_buffer;
  int32_t input_buffer_size;
} PrintJobArg;


void PrintFromInput(const int32_t worker_id, const void *input) {
  if (input == NULL) {
    return;
  }
  const PrintJobArg *arg = (const PrintJobArg *)input;

  if (arg->input_buffer == NULL) {
    fprintf(stderr, "PrintFromInput(): input is NULL");
    return;
  }

  const char *string = (const char *)arg->input_buffer;

  printf("Thread %d: %s\n", worker_id, string);
}

void PrintHello(const int32_t worker_id, const void *input) {
  printf("Thread %d: Hello!\n", worker_id);
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

  printf("Thread %d: Finished sleeping!\n", worker_id);
}

int main() {
  ThreadPool *thread_pool = ThreadPoolInit(2);
  int32_t concurrency = 0;
  concurrency =  pthread_getconcurrency();
  char *string = "I am here";
  int32_t size = 10;
  PrintJobArg input_arg = { (uint8_t *)string, size};

  ThreadpoolSubmit(thread_pool, PrintHello, NULL);
  ThreadpoolSubmit(thread_pool, PrintFromInput, &input_arg);
  ThreadpoolSubmit(thread_pool, PrintHello, NULL);
  ThreadpoolSubmit(thread_pool, PrintHello, NULL);
  ThreadpoolSubmit(thread_pool, SleepJob, NULL);

  log_info("Now waiting for jobs to complete...");
  ThreadpoolWaitAllJobs(thread_pool);

  ThreadpoolDestroy(thread_pool);

  return 0;
};
