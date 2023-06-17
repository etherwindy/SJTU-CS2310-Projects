#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int memorySize;

typedef struct region{
  int begin;
  int end;
  char *name;
  struct region *pre;
  struct region *next;
}region;

region *list;


void statusReport();
void request(char *name, size_t size, char strategy);
void release(char *name);
region *makeRegion(size_t begin, size_t end, char *name, region *pre, region *next);

/* Create neww region */
region *makeRegion(size_t begin, size_t end, char *name, region *pre, region *next) {
  region *newRegion = (region *) malloc(sizeof(region));
  newRegion->begin = begin;
  newRegion->end = end;
  if (strlen(name)) {
    newRegion->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
    strcpy(newRegion->name, name);
  }
  newRegion->pre = pre;
  newRegion->next = next;
  if (pre) pre->next = newRegion;
  if (next) next->pre = newRegion;
  return newRegion;
}

/* Report status */
void statusReport() {
  struct region *cur = list;
  while (cur) {
    printf("Address [%d:%d] ", cur->begin, cur->end);
    if (cur->name) printf("Process %s\n", cur->name);
    else printf("Unused\n");
    cur = cur->next;
  }
}


/* Deal with request */
void request(char *name, size_t size, char strategy) {
  struct region *destRegion = NULL;
  // First fit
  if (strategy == 'F') {
    destRegion = list;
    while (destRegion) {
      if (!destRegion->name && (destRegion->end - destRegion->begin + 1) >= size)
        break;
      destRegion = destRegion->next;
    }
  }
  // Best fit
  else if (strategy == 'B') {
    struct region *cur = list;
    while (cur) {
      size_t curSize = cur->end - cur->begin + 1;
      if (!cur->name && curSize >= size) {
        if (!destRegion) destRegion = cur;
        else if (curSize > destRegion->end - destRegion->begin + 1)
          destRegion = cur;
      }
      cur = cur->next;
    }
  }
  // Worst fit
  else if (strategy == 'W') {
    struct region *cur = list;
    while (cur) {
      size_t curSize = cur->end - cur->begin + 1;
      if (!cur->name && curSize >= size) {
        if (!destRegion) destRegion = cur;
        else if (curSize < destRegion->end - destRegion->begin + 1)
          destRegion = cur;
      }
      cur = cur->next;
    }
  }

  // No such target region or the region has been allocated
  if (!destRegion || destRegion->name) {
    printf("No available memory to allocate!\n");
    return;
  }
  // The size of destRegion is smaller than size
  if (destRegion->end - destRegion->begin + 1 < size) {
    printf("No enough memory size!\n");
    return;
  }
  // Name the region
  destRegion->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
  strcpy(destRegion->name, name);

  if (destRegion->end - destRegion->begin + 1 == size)
    return;
  // create new node
  destRegion->next = makeRegion(destRegion->begin + size, destRegion->end, "", destRegion, destRegion->next);
  destRegion->end = destRegion->begin + size - 1;

  return;
}

void release(char *name) {
  region *cur = list;
  int flag = 0;
  while (cur) {
    if (cur->name && !strcmp(cur->name, name)) {
      free(cur->name);
      cur->name = NULL;
      flag = 1;
    }
    if (!cur->name && cur->pre && !cur->pre->name) {
      region *temp = cur->pre;
      cur->pre = temp->pre;
      if (temp->pre)
        temp->pre->next = cur;
      cur->begin = temp->begin;
      free(temp);
    }
    if (!cur->next && cur->next && !cur->next->name) {
      region *temp = cur->next;
      cur->next = temp->next;
      if (temp->next)
        temp->next->pre = cur;
      cur->end = temp->end;
      free(temp);
    }

    if (!cur->pre)
      list = cur;

    cur = cur->next;
  }
  if (!flag)
    printf("No memory to release!\n");
}


int main(int argc, char *argv[]) {
  if (argc == 1) memorySize = 1 << 20;
  else if (argc == 2) memorySize = atoi(argv[1]);
  else if (argc > 2) {
    fprintf(stderr, "Usage: <executable> memorySize\n" );
    return 0;
  }

  list = (region *) malloc(sizeof(region));

  list->begin = 0;
  list->end = memorySize - 1;
  list->name = NULL;
  list->pre = list->next = NULL;

  int shouldrun = 1;
  while (shouldrun) {
    printf(">");

    char command[5];
    scanf("%s", command);

    if (!strcmp(command, "quit")) shouldrun = 0;
    else if (!strcmp(command, "RQ")) {
      char processName[10];
      int processSize;
      char strategy;
      scanf("%s %d %c", processName, &processSize, &strategy);
      processSize = (processSize % 4096) ? (((processSize >> 12) + 1) << 12) : ((processSize >> 12) << 12);
      request(processName, processSize, strategy);
    }
    else if (!strcmp(command, "RL")) {
      char processName[10];
      scanf("%s", processName);
      release(processName);
    }
    else if (!strcmp(command, "STAT"))
      statusReport();
  }

  return  0;
}
