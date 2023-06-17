#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

struct node *taskList;
struct node *newTaskList;
struct node *nextTask;

int num = 0;

int *response;
int *lastFinish;
int *wait;
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

Task *firstTask() {
  struct node *cur = taskList;
  Task *destTask = cur->task;
  delete(&taskList, destTask);
  return destTask;
}

// fetch a task to run according to round-robin algorithm
Task *taskToRun() {
  Task *destTask = nextTask->task;
  nextTask = nextTask->next ? nextTask->next : newTaskList;
  return destTask;
}

void rearrangement() {
  while(taskList) {
    Task *newTask = firstTask();
    insert(&newTaskList, newTask);
  }
}

void schedule() {
  response = (int *) malloc(num * sizeof(int));
  lastFinish = (int *) malloc(num * sizeof(int));
  wait = (int *) malloc(num * sizeof(int));

  memset(lastFinish, 0, num * sizeof(int));
  memset(wait, 0, num * sizeof(int));

  rearrangement();
  nextTask = newTaskList;

  while(newTaskList) {
    Task *task = taskToRun();
    int slice = task->burst < QUANTUM ? task->burst : QUANTUM;
    run(task, slice);

    if(!lastFinish[task->tid])
      response[task->tid] = curTime;

    wait[task->tid] += curTime - lastFinish[task->tid];
    curTime += slice;
    lastFinish[task->tid] = curTime;

    task->burst -= slice;
    if(!task->burst) {
      delete(&newTaskList, task);
      free(task->name);
      free(task);
    }
  }

  double averageTurnaround = 0;
  double averageResponse = 0;
  double averageWait = 0;

  for (int i = 0; i < num; ++i) {
    averageTurnaround += lastFinish[i];
    averageResponse += response[i];
    averageWait += wait[i];
  }
  averageTurnaround /= num;
  averageResponse /= num;
  averageWait /= num;
  printf("Average turnaround time: %lf\n", averageTurnaround);
  printf("Average response time: %lf\n", averageResponse);
  printf("Average waiting time: %lf\n", averageWait);

  free(response);
  free(lastFinish);
  free(wait);
}
