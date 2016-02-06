
#ifndef THREAD_POOL_C_H
#define THREAD_POOL_C_H

#include <stdint.h>

typedef struct ThreadPool ThreadPool;
typedef void(*JobFunction)(int32_t worker_id, const void *input);

ThreadPool *ThreadPoolInit(int32_t threads_num);

int32_t ThreadpoolSubmit(ThreadPool *thread_pool, JobFunction job,
  const void *input);

void ThreadpoolWaitAllJobs(ThreadPool *thread_pool);

void ThreadpoolDestroy(ThreadPool *thread_pool);

#endif