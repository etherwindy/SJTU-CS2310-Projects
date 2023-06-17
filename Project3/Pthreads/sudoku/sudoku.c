#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* structure for passing data to threads */
typedef struct
{
  int row;
  int column;
} parameters;

int puzzle[9][9]; //网格

int valid[11] = {1,1,1,1,1,1,1,1,1,1,1}; //前九位为九个子网格的合法性，后两位分别为行和列的合法性

void *checkRow(void *param);
void *checkColumn(void *param);
void *checkSubgrid(void *param);

//-----------------------------------------------
//判断行的合法性
void *checkRow(void *param)
{
  parameters *temp = (parameters*) param;
  int flag[10];
  for(int i = temp->row; i < 9; ++i)
  {
    memset(flag, 0, sizeof(flag));
    for(int j = temp->column; j < 9; ++j)
    {
      if(flag[puzzle[i][j]] == 0)
        flag[puzzle[i][j]] = 1;
      else
      {
        printf("Line %d is invalid!\n", i+1);
        valid[9] = 0;
        break;
      }
    }
  }
  pthread_exit(0);
}
//判断列的合法性
void *checkColumn(void *param)
{
  parameters *temp = (parameters *) param;
  int flag[10];
  for(int j = temp->column; j < 9; ++j)
  {
    memset(flag, 0, sizeof(flag));
    for(int i = temp->row; i < 9; ++i)
    {
      if(flag[puzzle[i][j]] == 0)
        flag[puzzle[i][j]] = 1;
      else
      {
        printf("Column %d is invalid!\n", j+1);
        valid[10] = 0;
        break;
      }
    }
  }
  pthread_exit(0);
}
//判断子网格的合法性
void *checkSubgrid(void *param)
{
  parameters *temp = (parameters *)param;
  int startRow = temp->row;
  int startColumn = temp->column;
  int flag[10] = {0};
  for(int i = startRow; i < startRow + 3; ++i)
  {
    for(int j = startColumn; j < startColumn + 3; ++j)
    {
      if(flag[puzzle[i][j]] == 0)
        flag[puzzle[i][j]] = 1;
      else
      {
        valid[startRow + startColumn/3] = 0;
        pthread_exit(0);
      }
    }
  }
  pthread_exit(0);
}

//----------------------------------------------------------------

int main()
{
  //读取数据
  FILE *file = fopen("sudoku.txt","r");
  if(!file)
  {
    printf("Fail to open sudoku.txt!\n");
    return 0;
  }
  for(int i = 0; i < 9; ++i)
    for(int j = 0; j < 9; ++j)
      fscanf(file, "%d", &puzzle[i][j]);

  fclose(file);

  //设置参数
  parameters *data[9];
  for(int i = 0; i < 9; ++i)
  {
    data[i] = (parameters *) malloc(sizeof(parameters));
    data[i]->row = i/3*3;
    data[i]->column = i%3*3;
  }

  pthread_t tidRow;
  pthread_t tidColumn;
  pthread_t tidSubgrid[9];

  //创建线程
  pthread_create(&tidRow, NULL, checkRow, data[0]);
  pthread_create(&tidRow, NULL, checkColumn, data[0]);
  for(int i = 0; i < 9; ++i)
    pthread_create(&tidSubgrid[i], NULL, checkSubgrid, data[i]);

  for(int i = 0; i < 9; ++i)
    pthread_join(tidSubgrid[i], NULL);
  pthread_join(tidRow, NULL);
  pthread_join(tidColumn, NULL);

  //判断是否合法
  int check = 1;
  for(int i = 0; i < 11; ++i)
    check &= valid[i];

  if(check)
    printf("Valid!\n");

  else {
    if(!valid[0]) printf("The upper left subgrid is invalid!\n");
    if(!valid[1]) printf("The upper subgrid is invalid!\n");
    if(!valid[2]) printf("The upper right subgrid is invalid!\n");
    if(!valid[3]) printf("The left subgrid is invalid!\n");
    if(!valid[4]) printf("The central subgrid is invalid!\n");
    if(!valid[5]) printf("The right subgrid is invalid!\n");
    if(!valid[6]) printf("The bottom left subgrid is invalid!\n");
    if(!valid[7]) printf("The bottom subgrid is invalid!\n");
    if(!valid[8]) printf("The bottom right subgrid is invalid!\n");
  }

  for(int i = 0; i < 9; ++i)
    free(data[i]);

  return 0;
}
