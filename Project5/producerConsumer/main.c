#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"

int main(int argc, char *argv[]) {
  /* 1. Get command line argument argv[1], argv[2], argv[3] */
  if(argc != 4) {
    fprintf(stderr, "Usage: <executable> sleepSecond numProducer numConsumer\n" );
    return 0;
  }
  unsigned int sleepTime = atoi(argv[1]);
  size_t producerNum = atoi(argv[2]), consumerNum = atoi(argv[3]);

  /* 2. Initialize buffer */
  init_buffer();

  pthread_t *producerTasks = (pthread_t *) malloc(producerNum * sizeof(pthread_t));
  pthread_t *consumerTasks = (pthread_t *) malloc(consumerNum * sizeof(pthread_t));
  int *producerId = (int *) malloc(producerNum * sizeof(int));
  int *consumerId = (int *) malloc(consumerNum * sizeof(int));

  /* 3. Create producer thread(s) */
  for (int i = 0; i < producerNum; ++i) {
    producerId[i] = i;
    pthread_create(&producerTasks[i], NULL, producer, &producerId[i]);
  }

  /* 4. Create consumer thread(s) */
  for (int i = 0; i < consumerNum; ++i) {
    consumerId[i] = i;
    pthread_create(&consumerTasks[i], NULL, consumer, &consumerId[i]);
  }

  /* 5. Sleep */
  sleep(sleepTime);

  /* 6. Exit */
  for (int i = 0; i < producerNum; ++i) {
    pthread_cancel(producerTasks[i]);
    pthread_join(producerTasks[i], NULL);
  }

  for (int i = 0; i < consumerNum; ++i) {
    pthread_cancel(consumerTasks[i]);
    pthread_join(consumerTasks[i], NULL);
  }

  free(producerTasks);
  free(consumerTasks);
  free(producerId);
  free(consumerId);
  destroy_buffer();
  return 0;
}
