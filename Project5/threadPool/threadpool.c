/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1
#define ERROR 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
  void (*function)(void *p);
  void *data;
}
task;

// mutex and semaphore
pthread_mutex_t mutex; // ensure only one thread can enter the critical section
sem_t full; // number of works in the queue
sem_t empty; // number of empty spaeces in the queue;

// the work queue
task workQueue[QUEUE_SIZE + 1];
size_t queueFront = 0, queueRear = 0;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
  pthread_mutex_lock(&mutex);
  if((queueRear + 1) % (QUEUE_SIZE + 1) == queueFront) {
    pthread_mutex_unlock(&mutex);
    return ERROR;
  }
  queueRear = (queueRear + 1) % (QUEUE_SIZE + 1);
  workQueue[queueRear] = t;
  pthread_mutex_unlock(&mutex);
  return 0;
}

// remove a task from the queue
task dequeue() 
{
  pthread_mutex_lock(&mutex);
  queueFront = (queueFront + 1) % (QUEUE_SIZE + 1);
  task worktodo = workQueue[queueFront];
  pthread_mutex_unlock(&mutex);
  return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
  task worktodo;
  while(TRUE) {
    sem_wait(&full);
    worktodo = dequeue();
    sem_post(&empty);
    // execute the task
    execute(worktodo.function, worktodo.data);
  }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
  (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
  task newWork;
  newWork.function = somefunction;
  newWork.data = p;

  sem_wait(&empty);
  if(enqueue(newWork))
    return ERROR;
  sem_post(&full);

  return 0;
}

// initialize the thread pool
void pool_init(void)
{
  /* Create and initialize the mutex lock */
  pthread_mutex_init(&mutex, NULL);
  /* Create the initialize the semaphores */
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, QUEUE_SIZE);

  for(int i = 0; i < NUMBER_OF_THREADS; ++i) {
    pthread_create(&bee[i],NULL,worker,NULL);
  }
}

// shutdown the thread pool
void pool_shutdown(void)
{
  for(int i = 0; i < NUMBER_OF_THREADS; ++i) {
    pthread_cancel(bee[i]);
    pthread_join(bee[i],NULL);
  }
  sem_destroy(&full);
  sem_destroy(&empty);
  pthread_mutex_destroy(&mutex);
}
