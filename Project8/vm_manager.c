#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OFFSET_BITS 8
#define PAGE_BITS 8
#define PAGE_SIZE (1 << OFFSET_BITS)
#define PAGE_NUM (1 << PAGE_BITS)
#define FRAME_NUM 256
#define TLB_SIZE 16

FILE *backing_store, *file;

int total_cnt = 0;
int TLB_miss_cnt = 0;
int page_fault_cnt = 0;
int next_frame = 0;
int next_TLB_index = 0;

typedef struct TLBItem {
  uint8_t pageNumber;
  uint8_t frameNumber;
  uint8_t valid;
} TLBItem;

typedef struct pageTableItem {
  uint16_t frameNumber;
  uint8_t valid;
} pageTableItem;

TLBItem TLB[TLB_SIZE];
pageTableItem page_table[PAGE_NUM];
int8_t physical_memory[FRAME_NUM * PAGE_SIZE];

uint32_t translate_address(uint32_t logical);
int check_TLB(uint8_t pageNumber, uint8_t *frameNumber);
void page_fault(uint8_t pageNumber);
uint8_t get_frame();
uint8_t access_memory(uint16_t physical);
void update_TLB(uint8_t pageNumber, uint8_t frameNumber);
void update_page_table(uint8_t pageNumber, uint8_t frameNumber);

/* Translate logical addresss to phisical address */
uint32_t translate_address(uint32_t logical) {
  total_cnt++;
  uint8_t offset = logical % (1 << 8);
  uint8_t pageNumber = (logical >> 8) % (1 << 8);
  uint8_t frameNumber = 0;
  // TLB miss
  if (!check_TLB(pageNumber, &frameNumber)) {
    TLB_miss_cnt++;
    // page fault
    if (!page_table[pageNumber].valid) {
      page_fault(pageNumber);
    }
    frameNumber = page_table[pageNumber].frameNumber;
    // update TLB
    update_TLB(pageNumber, frameNumber);
  }

  uint16_t physical = (frameNumber << 8) + offset;
  return physical;
}

/* Check whether the page is in TLB */
int check_TLB(uint8_t pageNumber, uint8_t *frameNumber) {
  for (int i = 0; i < TLB_SIZE; ++i) {
    if (TLB[i].pageNumber == pageNumber) {
      if (TLB[i].valid) {
        *frameNumber = TLB[i].frameNumber;
        return 1;
      }
    }
  }
  return 0;
}

/* Deal with page fault */
void page_fault(uint8_t pageNumber) {
  page_fault_cnt++;
  int32_t frameNumber = get_frame();
  fseek(backing_store, pageNumber * PAGE_SIZE, SEEK_SET);
  fread(physical_memory + frameNumber * PAGE_SIZE, sizeof(int8_t), PAGE_SIZE, backing_store);
  for (int i = 0; i < PAGE_NUM; ++i) {
    if (page_table[i].valid && page_table[i].frameNumber == frameNumber) {
      page_table[i].valid = 0;
      break;
    }
  }
  page_table[pageNumber].frameNumber = frameNumber;
  page_table[pageNumber].valid = 1;
  memset(TLB, 0, TLB_SIZE * sizeof(TLBItem));
  next_TLB_index = 0;
}

/* Use FCFS algortithm to fetch a frame */
uint8_t get_frame() {
  uint8_t frameNumber = next_frame;
  next_frame = (next_frame + 1) % FRAME_NUM;
  return frameNumber;
}

/* Update TLB */
void update_TLB(uint8_t pageNumber, uint8_t frameNumber) {
  TLB[next_TLB_index].pageNumber = pageNumber;
  TLB[next_TLB_index].frameNumber = frameNumber;
  TLB[next_TLB_index].valid = 1;
  next_TLB_index = (next_TLB_index + 1) % TLB_SIZE;
}

/* Access memory */
uint8_t access_memory(uint16_t physical) {
  return physical_memory[physical];
}

int main() {
  file = fopen("addresses.txt", "r");
  if (!file) {
    printf("Fail to open addresses.txt\n");
    return 0;
  }
  backing_store = fopen("BACKING_STORE.bin", "r");
  if (!backing_store) {
    printf("Fail to open BACKING_STORE.bin\n");
    return 0;
  }

  memset(TLB, 0, TLB_SIZE * sizeof(TLBItem));
  memset(page_table, 0, PAGE_NUM * sizeof(pageTableItem));
  memset(physical_memory, 0, FRAME_NUM * PAGE_SIZE * sizeof(int8_t));

  uint32_t logical;
  while (fscanf(file, "%d", &logical) != EOF) {
    uint16_t physical = translate_address(logical);
    uint8_t value = access_memory(physical);
    printf("logical address: %u, physical address: %u, value: %02x\n", logical, physical, value);
  }

  printf("page fault rate: %f\n", (float)page_fault_cnt / total_cnt);
  printf("TLB hit rate: %f\n", (float)(total_cnt - TLB_miss_cnt) / total_cnt);

  fclose(backing_store);
  fclose(file);
  return 0;
}
