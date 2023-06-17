#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------
int len = 0; //序列长度
int *array; //目标序列

/* structure for passing data to threads */
typedef struct
{
  int left;
  int right; //子序列范围为[left, right)
} parameters;

int cmp(const void *a, const void *b);
void *partSort(void *param);
void *mergeSort(void *param);

//------------------------------------------------------------------
int cmp(const void *a, const void *b)
{
  return *(int*)a-*(int*)b;
}

//使用quicksort对子序列进行排序
void *partSort(void *param)
{
  parameters *temp = (parameters *)param;
  qsort(array+temp->left, temp->right-temp->left, sizeof(int), cmp);
  pthread_exit(0);
}

//使用mergesort将左右两个子序列合并
void *mergeSort(void *param)
{
  //创建两个用于临时存储子序列的数组
  int *low = (int *) malloc(len/2 * sizeof(int));
  int *high = (int *) malloc((len-len/2) * sizeof(int));
  for(int i = 0; i < len/2; ++i)
    low[i] = array[i];
  for(int i = len/2; i < len; ++i)
    high[i-len/2] = array[i];

  int cur, cur1, cur2;
  cur = cur1 = cur2 = 0;

  //将两个序列最小的元素存入原序列，直到其中一个子序列读完
  while(cur1 != len/2 && cur2 != len-len/2)
  {
    if(low[cur1] < high[cur2])
      array[cur++] = low[cur1++];
    else
      array[cur++] = high[cur2++];
  }

  //将剩余的子序列存入原序列
  while(cur1 < len/2)
  {
    array[cur++] = low[cur1++];
  }
  while(cur2 < len-len/2)
  {
    array[cur++] = high[cur2++];
  }
  free(low);
  free(high);
  pthread_exit(0);
}

//---------------------------------------------------------------------
int main()
{
  //打开文件
  FILE *file = fopen("sort.txt", "r");
  if(!file)
  {
    printf("Fail to open sort.txt!\n");
    return 0;
  }
  int tmp;
  while(fscanf(file, "%d", &tmp) != EOF)
    len++;

  rewind(file);
  array = (int *) malloc(len * sizeof(int));
  for(int i = 0; i < len; ++i)
    fscanf(file, "%d", &array[i]);

  //设置参数
  parameters *leftrange = (parameters *) malloc(sizeof(parameters));
  parameters *rightrange = (parameters *) malloc(sizeof(parameters));
  leftrange->left = 0;
  leftrange->right = len/2;
  rightrange->left = len/2;
  rightrange->right = len;

  pthread_t tidLeft;
  pthread_t tidRight;
  pthread_t tidMerge;

  //创建线程
  pthread_create(&tidLeft, NULL, partSort, leftrange);
  pthread_create(&tidRight, NULL, partSort, rightrange);

  pthread_join(tidLeft, NULL);
  pthread_join(tidRight, NULL);

  pthread_create(&tidMerge, NULL, mergeSort, NULL);
  pthread_join(tidMerge, NULL);

  //输出结果
  for(int i = 0; i < len; ++i)
    printf("%d ", array[i]);
  printf("\n");

  free(leftrange);
  free(rightrange);
  free(array);
  return 0;
}
