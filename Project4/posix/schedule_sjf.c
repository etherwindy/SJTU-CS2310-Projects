#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

int num = 0;

struct node *taskList;

int *response;
int *turnaround;
int curTime = 0;

void add(char *name, int priority, int burst) {
  Task *newTask = (Task *) malloc(sizeof(Task));
  // allocate memory to copy the name
  newTask->name = (char *) malloc((strlen(name)+1) * sizeof(char));
  strcpy(newTask->name, name);
  newTask->tid = num++;
  newTask->priority = priority;
  newTask->burst = burst;
  // insert the new task into the list
  insert(&taskList, newTask);
}

// fetch a task to run according to SJF algorithm
Task *taskToRun() {
  Task *destTask = taskList->task;
  struct node *cur = taskList;
  while(cur) { // find the task with the shortest burst
    if(cur->task->burst < destTask->burst)
      destTask = cur->task;
    cur = cur->next;
  }
  return destTask;
}

void schedule() {
  response = (int *) malloc(num * sizeof(int));
  turnaround = (int *) malloc(num * sizeof(int));

  while(taskList) {
    Task *task = taskToRun();
    run(task, task->burst);

    response[task->tid] = curTime;
    curTime += task->burst;
    turnaround[task->tid] = curTime;

    delete(&taskList, task);
    free(task->name);
    free(task);
  }
  double averageTurnaround = 0;
  double averageResponse = 0;
  double averageWait = 0;

  for (int i = 0; i < num; ++i) {
    averageTurnaround += turnaround[i];
    averageResponse += response[i];
  }
  averageTurnaround /= num;
  averageResponse /= num;
  averageWait = averageResponse;
  printf("Average turnaround time: %lf\n", averageTurnaround);
  printf("Average response time: %lf\n", averageResponse);
  printf("Average waiting time: %lf\n", averageWait);

  free(response);
  free(turnaround);
}
