
#include <pthread.h>
#include <stdio.h>
#include "thread_pool_c.h"
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "dbg.h"

#ifndef THPOOL_DEBUG
#define THPOOL_DEBUG 1
#else
#define THPOOL_DEBUG 0
#endif

volatile int32_t threads_keepalive;
volatile int32_t threads_on_hold;

typedef struct Job {
  struct Job *prev_job;
  JobFunction job_routine;
  const void *input;
} Job;

typedef struct JobsQueue {
  pthread_mutex_t mut;
  pthread_cond_t cond;
  Job *front;
  Job *rear;
  int32_t lenght;
  // This flag is used when a thread waiting on the cond. var of the queue
  // needs to be waken up because the process is exiting.
  int32_t program_end_flag;
} JobsQueue;

typedef struct Thread {
  int32_t id;
  pthread_t pthread;
  struct ThreadPool *thread_pool;
} Thread;

typedef struct ThreadPool {
  Thread **threads;
  volatile int32_t threads_alive_num;
  volatile int32_t threads_working_num;
  pthread_mutex_t thread_counts_mut;
  pthread_cond_t wait_threads_cond;
  JobsQueue *jobsqueue;
} ThreadPool;


int32_t ThreadInit(Thread **thread, int32_t id, ThreadPool *thread_pool);
void *WorkerThread(Thread *thread);
void ThreadCancelRoutine(void *thread);
void ThreadJoinAndDestroy(Thread *thread);
int32_t JobsQueueInit(JobsQueue **jobsqueue);
Job *JobsQueueWaitAndPop(JobsQueue *jobsqueue);
Job *JobsQueueTryPop(JobsQueue *jobsqueue);
void JobsQueuePush(JobsQueue *jobsqueue, Job *new_job);
void JobsQueueClear(JobsQueue *jobsqueue);
void JobsQueueAwakeThreads(JobsQueue *jobsqueue);


ThreadPool *ThreadPoolInit(int32_t threads_num) {
  threads_keepalive = 1;

  if (threads_num < 0) {
    threads_num = 0;
  }

  ThreadPool *thread_pool = NULL;

  // Initialise thread pool
  thread_pool = (ThreadPool *)malloc(sizeof(ThreadPool));
  check_mem(thread_pool);

  thread_pool->threads_alive_num = 0;
  thread_pool->threads_working_num = 0;

  // Initialise the jobs queue
  int32_t rc = JobsQueueInit(&thread_pool->jobsqueue);
  check(rc != -1, "ThreadPoolInit(): Couldn't initialise jobs queue");

  // Create the threads array in the threads pool
  thread_pool->threads = (Thread **)malloc(threads_num * sizeof(Thread *));
  check_mem(thread_pool->threads);

  // Setup mutex and condition variable
  pthread_mutex_init(&(thread_pool->thread_counts_mut), NULL);
  pthread_cond_init(&(thread_pool->wait_threads_cond), NULL);

  // Initialise worker threads
  for (int32_t i = 0; i < threads_num; i++) {
    rc = ThreadInit(&thread_pool->threads[i], i, thread_pool);
    check(rc != -1, "ThreadPoolInit(): Couldn't init thread %d", i);
  }

  // Wait for the threads to be initialised
  while (thread_pool->threads_alive_num < threads_num) {}

  return thread_pool;

error:
  if (thread_pool != NULL) {
    if (thread_pool->threads != NULL) {
      // Stop working threads
      threads_keepalive = 0;

      // Give one second to kill idle threads
      double TIMEOUT = 1.0;
      time_t start, end;
      double tpassed = 0.0;
      time (&start);
      while (tpassed < TIMEOUT && thread_pool->threads_alive_num){
        time (&end);
        tpassed = difftime(end,start);
      }
      for (int32_t j = 0; j < threads_num; j++) {
        if (thread_pool->threads[j] != NULL) {
          ThreadJoinAndDestroy(thread_pool->threads[j]);
        }
     } 
     free(thread_pool->threads);
    }
    if (thread_pool->jobsqueue != NULL) {
      JobsQueueClear(thread_pool->jobsqueue);
      free(thread_pool->jobsqueue);
    }
    pthread_mutex_destroy(&(thread_pool->thread_counts_mut));
    pthread_cond_destroy(&(thread_pool->wait_threads_cond));
    free(thread_pool);
  }

  return NULL;
}


int32_t ThreadInit(Thread **thread, int32_t id, ThreadPool *thread_pool) {
  *thread = (Thread *)malloc(sizeof(Thread));
  check_mem(*thread);

  (*thread)->thread_pool = thread_pool;
  (*thread)->id = id;

  int32_t rc = pthread_create(&(*thread)->pthread, NULL, WorkerThread,
                                  (*thread));
  check(rc == 0, "ThreadInit(): couldn't create pthread. Return code is %d", rc);

  return 0; 

error:
  return -1;
}

void ThreadJoinAndDestroy(Thread *thread) {
  int32_t rc = pthread_join(thread->pthread, NULL);
  check(rc == 0, "ThreadJoinAndDestroy(): Couldn't join thread")

  if(thread != NULL) {
    free(thread);
    thread = NULL;
  }

error:
  if(thread != NULL) free(thread);
}

int32_t ThreadpoolSubmit(ThreadPool *thread_pool, JobFunction job,
  const void *input) {
  Job *new_job = (Job *)malloc(sizeof(Job));
  check_mem(new_job);

  // Init job
  new_job->job_routine = job;
  new_job->input = input;

  // Push job on the queue
  JobsQueuePush(thread_pool->jobsqueue, new_job);

  return 0;

error:
  return -1;
}

void ThreadpoolWaitAllJobs(ThreadPool *thread_pool) {
  pthread_mutex_lock(&(thread_pool->thread_counts_mut));
  while (thread_pool->threads_working_num > 0) {
    pthread_cond_wait(&(thread_pool->wait_threads_cond),
                      &(thread_pool->thread_counts_mut));
  }

  pthread_mutex_unlock(&(thread_pool->thread_counts_mut));
}

void ThreadpoolDestroy(ThreadPool *thread_pool) {
  if (thread_pool == NULL) {
    return;
  }

  volatile int32_t threads_total = thread_pool->threads_alive_num;

  // Stop worker threads' function
  //threads_keepalive = 0;

  //// Give one second to kill idle threads
  //double TIMEOUT = 1.0;
  //time_t start, end;
  //double tpassed = 0.0;
  //time (&start);
  //while (tpassed < TIMEOUT && thread_pool->threads_alive_num == 0) {
  //  time (&end);
  //  tpassed = difftime(end,start);
  //}

  // Cancel all the threads
  for (int32_t i = 0; i < threads_total; i++) {
    pthread_cancel(thread_pool->threads[i]->pthread);
  }
  // Cleanup the threads list
  for (int32_t i = 0; i < threads_total; i++) {
    pthread_join(thread_pool->threads[i]->pthread, NULL);
    //ThreadJoinAndDestroy(thread_pool->threads[i]);
  }
  free(thread_pool->threads);

  // Cleanup the queue
  JobsQueueClear(thread_pool->jobsqueue);
  free(thread_pool->jobsqueue);
  thread_pool->jobsqueue = NULL;

  // Cleanup the thread pool
  pthread_mutex_destroy(&(thread_pool->thread_counts_mut));
  pthread_cond_destroy(&(thread_pool->wait_threads_cond));
  free(thread_pool);
  thread_pool = NULL;
}

void *WorkerThread(Thread *thread) {
  ThreadPool *thread_pool = thread->thread_pool;


  // Mark thread as initialised
  pthread_mutex_lock(&(thread_pool->thread_counts_mut));
  thread_pool->threads_alive_num ++;
  pthread_mutex_unlock(&(thread_pool->thread_counts_mut));

  // Setup cancellation policy
  int32_t last_state, last_type;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &last_state);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &last_type);
  pthread_cleanup_push(ThreadCancelRoutine, thread);

  log_info("Initialised thread %d", thread->id);

  while (threads_keepalive) {
    debug("Thread %d: About to enter wait and pop", thread->id);
    Job *job = JobsQueueWaitAndPop(thread_pool->jobsqueue);

    if (threads_keepalive) {
      JobFunction job_routine = NULL;
      const void *input = NULL;
      // If the job pulled is valid, execute it, then free it
      if (job != NULL) {
        job_routine = job->job_routine;
        input = job->input;

        // Increase counter of threads working on a job
        pthread_mutex_lock(&(thread_pool->thread_counts_mut));
        thread_pool->threads_working_num ++;
        pthread_mutex_unlock(&(thread_pool->thread_counts_mut));

        job_routine(thread->id, input);

        free(job);

        debug("Finished job, thread %d", thread->id);
        // Decrease counter of threads working on a job
        pthread_mutex_lock(&(thread_pool->thread_counts_mut));
        thread_pool->threads_working_num --;
        if (thread_pool->threads_working_num == 0) {
          pthread_cond_signal(&(thread_pool->wait_threads_cond));
        }
        pthread_mutex_unlock(&(thread_pool->thread_counts_mut));
      }
    }

    pthread_testcancel();
  }

  // Mark thread as dead
  pthread_mutex_lock(&(thread_pool->thread_counts_mut));
  thread_pool->threads_alive_num --;
  pthread_mutex_unlock(&(thread_pool->thread_counts_mut));
  log_warn("Shouldn't be here");
  log_info("Exiting thread %d", thread->id);
  pthread_cleanup_pop(0);
  pthread_exit(NULL);
}

void ThreadCancelRoutine(void *input) {
  Thread *thread = (Thread *)input;

  // Mark thread as dead
  pthread_mutex_lock(&(thread->thread_pool->thread_counts_mut));
  thread->thread_pool->threads_alive_num --;
  pthread_mutex_unlock(&(thread->thread_pool->thread_counts_mut));

  // Check to see if the mutex on the queue is still held; if so,
  // unlock it
  if (pthread_mutex_trylock(&(thread->thread_pool->jobsqueue->mut))) {
    // Unlock again
    pthread_mutex_unlock(&(thread->thread_pool->jobsqueue->mut));
  }
  else {
    pthread_mutex_unlock(&(thread->thread_pool->jobsqueue->mut));
  }

  log_info("Exiting thread %d", thread->id);
}

int32_t JobsQueueInit(JobsQueue **jobsqueue) {
  JobsQueue *queue = (JobsQueue *)malloc(sizeof(JobsQueue));
  check_mem(queue);

  queue->lenght = 0;
  queue->front = NULL;
  queue->rear = NULL;
  pthread_mutex_init(&(queue->mut), NULL);
  pthread_cond_init(&(queue->cond), NULL);
  queue->program_end_flag = 0;

  *jobsqueue = queue;

  return 0;

error:
  return -1;
}

Job *JobsQueueWaitAndPop(JobsQueue *jobsqueue) {
  // While the queue is empty, make thread sleep
  debug("About to lock mutex");
  pthread_mutex_lock(&(jobsqueue->mut));
  while (jobsqueue->lenght <= 0 || jobsqueue->program_end_flag) {
    debug("Waiting in wait and pop");
    pthread_cond_wait(&(jobsqueue->cond), &(jobsqueue->mut));
  }
  if (jobsqueue->program_end_flag) {
    debug("program_end_flag set");
    return NULL;
  }

  Job *popped_job = jobsqueue->front;

  switch (jobsqueue->lenght) {
    case 0: {
      break;
    }
    case 1: {
      jobsqueue->front = NULL;
      jobsqueue->rear = NULL;
      jobsqueue->lenght = 0;
      break;
    }
    default: {
      jobsqueue->front = popped_job->prev_job;
      jobsqueue->lenght --;
    }
  }

  pthread_mutex_unlock(&(jobsqueue->mut));

  return popped_job;
}

Job *JobsQueueTryPop(JobsQueue *jobsqueue) {
  pthread_mutex_lock(&(jobsqueue->mut));

  Job *popped_job = jobsqueue->front;

  switch (jobsqueue->lenght) {
    case 0: {
      break;
    }
    case 1: {
      jobsqueue->front = NULL;
      jobsqueue->rear = NULL;
      jobsqueue->lenght = 0;
      break;
    }
    default: {
      jobsqueue->front = popped_job->prev_job;
      jobsqueue->lenght --;
    }
  }

  pthread_mutex_unlock(&(jobsqueue->mut));

  return popped_job;
}

void JobsQueuePush(JobsQueue *jobsqueue, Job *new_job) {
  pthread_mutex_lock(&(jobsqueue->mut));

  new_job->prev_job = NULL;

  switch (jobsqueue->lenght) {
    case 0: {
      jobsqueue->front = new_job;
      jobsqueue->rear = new_job;
      break;
    }
    default: {
      jobsqueue->rear->prev_job = new_job;
      jobsqueue->rear = new_job;
    }
  }
  jobsqueue->lenght ++;

  // Inform workers of the availability of a job
  pthread_cond_signal(&(jobsqueue->cond));

  pthread_mutex_unlock(&(jobsqueue->mut));
}

void JobsQueueAwakeThreads(JobsQueue *jobsqueue) {
  // Set the flag to be 1
  pthread_mutex_lock(&(jobsqueue->mut));
  jobsqueue->program_end_flag = 1;
  pthread_mutex_unlock(&(jobsqueue->mut));

  // Send message to all threads waiting on the cond variable
  pthread_cond_broadcast(&(jobsqueue->cond));
}

void JobsQueueClear(JobsQueue *jobsqueue) {
  while (jobsqueue->lenght) {
    Job *job_to_free = JobsQueueTryPop(jobsqueue);
    if (job_to_free != NULL) {
      free(job_to_free);
    }
  }

  jobsqueue->front = NULL;
  jobsqueue->rear = NULL;
  jobsqueue->lenght = 0;
  jobsqueue->program_end_flag = 0;
}
