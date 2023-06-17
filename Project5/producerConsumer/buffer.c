#include <stdlib.h> /* required for rand() */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

#define true 1
#define MAX_SLEEP_TIME 3
#define BUFFER_SIZE 10

typedef int buffer_item;

//----------------------------------------------------

/* Implement buffer as a queue */
buffer_item buffer[BUFFER_SIZE + 1];
int front = 0, rear = 0;

// mutex and semaphores
pthread_mutex_t mutex;
sem_t full;
sem_t empty;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);

//-----------------------------------------------------

/* public functions */

/* Initialize mutex and semaphores */
void init_buffer() {
  pthread_mutex_init(&mutex, NULL);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, BUFFER_SIZE);
}

/* Release mutex nd semaphores */
void destroy_buffer() {
  pthread_mutex_destroy(&mutex);
  sem_destroy(&full);
  sem_destroy(&empty);
}

/* producer thread */
void *producer(void *param) {
  buffer_item item;

  while (true) {
    /* sleep for a random period of time */
    sleep(rand() % MAX_SLEEP_TIME + 1);
    /* generate a random number */
    item = rand();
    if (insert_item(item))
      fprintf(stderr, "report error condition");
    else
      printf("producer %d produced %d\n", *(int *)param, item);
  }
}

// consumer thread
void *consumer(void *param) {
  buffer_item item;

  while(true) {
    /* sleep for a ramdom period of time */
    sleep(rand() % MAX_SLEEP_TIME + 1);
    if (remove_item(&item))
      fprintf(stderr, "report error condition");
    else
      printf("concumer %d consumed %d\n", *(int *)param, item);
  }
}

//-----------------------------------------------------

/* private functions */

/* insert item to the buffer */
int insert_item(buffer_item item) {
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);

  // critical section

  // Report error if the buffer is full
  if ((rear + 1) % (BUFFER_SIZE + 1) == front) {
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  // insert item at the rear of the buffer
  rear = (rear + 1) % (BUFFER_SIZE + 1);
  buffer[rear] = item;

  pthread_mutex_unlock(&mutex);
  sem_post(&full);

  return 0;
}

/* Remove item from the buffer */
int remove_item(buffer_item *item) {
  sem_wait(&full);
  pthread_mutex_lock(&mutex);

  // critical section

  // Reprot error if the buffer is empty
  if (rear == front) {
    pthread_mutex_unlock(&mutex);
    return 1;
  }
  // Remove item from the front of the buffer
  front = (front + 1) % (BUFFER_SIZE + 1);
  *item = buffer[front];

  pthread_mutex_unlock(&mutex);
  sem_post(&empty);

  return 0;
}
