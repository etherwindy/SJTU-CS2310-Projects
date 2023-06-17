#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* user request resources */
int request_resources(int customer_num, int request[]);
/* user release resource */
int release_resources(int customer_num, int release[]);


void print_information();
int safe();
int vleq(int *arr1, int *arr2, int n);

int should_run = 1;

int main(int argc, char *argv[]) {
  /* Initialize global variables */
  //
  //
  /* Read available[] from command line */
  if (argc != 5) {
    fprintf(stderr, "Usage: <executable> sleepSecond numProducer numConsumer\n" );
    return 0;
  }
  for (int i = 0; i < 4; ++i) 
    available[i] = atoi(argv[i + 1]);

  /* Read maximum[][] from input.txt */
  FILE *file = fopen("input.txt", "r");
  if (!file) {
    printf("Fail to open input.txt!\n");
    return 0;
  }

  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 4; ++j) {
      fscanf(file, "%d", &maximum[i][j]);
      need[i][j] = maximum[i][j];
    }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    memset(allocation[i], 0, sizeof(allocation[i]));

  while (should_run) {
    printf(">");
    char command[5];

    scanf("%s", command);

    if (!strcmp(command, "quit"))
      should_run = 0;

    else if (!strcmp(command, "*"))
      print_information();

    else if (!strcmp(command, "RQ")) {
      int customer_num;
      int resources[NUMBER_OF_RESOURCES];

      scanf("%d", &customer_num);
      for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        scanf("%d", &resources[i]);

      int err = request_resources(customer_num, resources);

      if (err == -1)
        printf("Invalid customer number!\n");
      else if (err == -2)
        printf("Insufficient available resources!\n");
      else if (err == -3)
        printf("%d customer doesn't need so many resources!\n", customer_num);
      else if (err == 1)
        printf("The state is not safe!\n");
      else
        printf("Successfully allocate the resources!\n");
    }

    else if (!strcmp(command, "RL")) {
      int customer_num;
      int resources[NUMBER_OF_RESOURCES];

      scanf("%d", &customer_num);
      for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        scanf("%d", &resources[i]);

      int err = release_resources(customer_num, resources);

      if (err == -1)
        printf("Invalid customer number!\n");
      else if (err == 1)
        printf("%d customer doesn't have so many resources!\n", customer_num);
      else
        printf("Successfully release tht resources!\n");
    }
  }

  return 0;
}

void print_information() {
  printf("available array is:\n");
  for (int i = 0; i < NUMBER_OF_RESOURCES - 1; ++i)
    printf("%d ", available[i]);
  printf("%d\n", available[NUMBER_OF_RESOURCES - 1]);

  printf("maximum matrix is:\n");
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
    for (int j = 0; j < NUMBER_OF_RESOURCES - 1; ++j)
      printf("%d ", maximum[i][j]);
    printf("%d\n", maximum[i][NUMBER_OF_RESOURCES - 1]);
  }

  printf("allocation matrix is:\n");
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
    for (int j = 0; j < NUMBER_OF_RESOURCES - 1; ++j)
      printf("%d ", allocation[i][j]);
    printf("%d\n", allocation[i][NUMBER_OF_RESOURCES - 1]);
  }

  printf("need matrix is:\n");
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
    for (int j = 0; j < NUMBER_OF_RESOURCES - 1; ++j)
      printf("%d ", need[i][j]);
    printf("%d\n", need[i][NUMBER_OF_RESOURCES - 1]);
  }
}

int request_resources(int customer_num, int resources[]) {
  if (customer_num > NUMBER_OF_CUSTOMERS)
    return -1;

  if (!vleq(resources, available, NUMBER_OF_RESOURCES))
    return -2;

  if (!vleq(resources, need[customer_num], NUMBER_OF_RESOURCES))
    return -3;

  for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
    available[i] -= resources[i];
    allocation[customer_num][i] += resources[i];
    need[customer_num][i] -= resources[i];
  }

  if (!safe()) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
      available[i] += resources[i];
      allocation[customer_num][i] -= resources[i];
      need[customer_num][i] += resources[i];
    }
    return 1;
  }

  return 0;
}

int release_resources(int customer_num, int resources[]) {
  if (customer_num >= NUMBER_OF_CUSTOMERS)
    return -1;

  if (!vleq(resources, allocation[customer_num], NUMBER_OF_RESOURCES))
      return 1;

  for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
    available[i] += resources[i];
    allocation[customer_num][i] -= resources[i];
    need[customer_num][i] += resources[i];
  }
  return 0;
}

int safe() {
  int work[NUMBER_OF_RESOURCES], finish[NUMBER_OF_CUSTOMERS] = {0};

  memcpy(work, available, NUMBER_OF_RESOURCES * sizeof(int));

  int customer_num = 0;
  while (customer_num < NUMBER_OF_CUSTOMERS) {
    for (customer_num = 0; customer_num < NUMBER_OF_CUSTOMERS; ++customer_num) {
      if (!finish[customer_num] && vleq(need[customer_num], work, NUMBER_OF_RESOURCES)) {
        for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
          work[i] += allocation[customer_num][i];

        finish[customer_num] = 1;
        break;
      }
    }
  }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    if (!finish[i])
      return 0;

  return 1;
}

int vleq(int *arr1, int *arr2, int n) {
  for (int i = 0; i < n; ++i)
    if (arr1[i] > arr2[i])
      return 0;
  return 1;
}

