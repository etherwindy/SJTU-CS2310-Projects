/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

void handler(int sig){
  int status = 0;
  pid_t pid = 0;
  while ((pid = waitpid(-1,&status,WNOHANG)) >0);
  return;
}

int main(void) {
  char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
  char *buffer[MAX_LINE/2 + 1]; // 上一条指令
  char *temp[MAX_LINE/2 + 1]; // 最终执行的指令（删去后台执行和重定向功能）
  char *father[MAX_LINE/2 + 1]; // 管道通信时父进程执行的指令
  char *child[MAX_LINE/2 + 1]; // 管道通信时子进程执行的指令
  char *filename; // 重定向时的文件名
  int should_run = 1; // 是否循环
  int buffer_len = 0; // buffer中指令的字符串长度

  for(int i = 0; i < MAX_LINE/2 + 1; ++i) {
    args[i] = buffer[i] = temp[i] = father[i] = child[i] = NULL;
  }

  while(should_run) {
    printf("osh>");
    fflush(stdout);
    int redirect = 0; // 是否重定向
    int ifpara = 0; // 是否后台执行
    int ifpipe = 0; // 是否管道通信
    int line_position = 0; // 重定向时‘|’字符在指令字符串中的位置

    char c = ' '; // 检测下一个字符是否为空格
    int len = 0;

    while(c == ' ') {
      char *str = (char*)malloc(1<<7);
      scanf("%s", str);
      c = getchar();
      args[len++] = str;
    }

    args[len] = NULL;

    if(!args[0]) continue;

    // 如果输入为"exit"，结束循环
    if(!strcmp(args[0], "exit") && !args[1]) {
      should_run = 0;
      continue;
    }

    // 如果输入为"!!"，从buffer中取指令
    if(!strcmp(args[0], "!!") && !args[1]) {
      if(!buffer[0]) printf("No commands in history.\n");

      else {
        char *ad = buffer[buffer_len-1]; // 临时存储指令最后一个字符
        if(!strcmp(buffer[buffer_len-1], "&")) {
          buffer[buffer_len-1] = NULL; // 若该字符为'&'，则暂时删去，方便将指令复制到temp
          ifpara = 1;
        }
        // 判断是否有重定向获管道通信
        int i;
        for(i = 0; buffer[i] && i < MAX_LINE/2 + 1; ++i) {

          if(!strcmp(buffer[i], ">")) {
            redirect = 1;
            break;
          }

          else if(!strcmp(buffer[i], "<")) {
            redirect = 2;
            break;
          }

          else if(!strcmp(buffer[i], "|")) {
            ifpipe = 1;
            line_position = i;
          }

          temp[i] = buffer[i];

        }
        temp[i] = NULL;
        if(ifpara) buffer[buffer_len-1] = ad; // 如果有后台执行，则补回最后一个字符，便于复制到buffer中
        if(redirect) filename = buffer[i + 1]; // 如果有重定向，读入文件名
      }

    }

    else {
      char *ad = args[len-1]; // 临时存储指令最后一个字符
      if(!strcmp(args[len-1], "&")) {
        args[len-1] = NULL; // 若该字符为'&'，则暂时删去，方便将指令复制到temp
        ifpara = 1;
      }
      // 判断是否有重定向或管道通信
      int i;
      for(i = 0; args[i] && i < MAX_LINE/2 + 1; ++i) {

        if(!strcmp(args[i], ">")) {
          redirect = 1;
          break;
        }

        else if(!strcmp(args[i], "<")) {
          redirect = 2;
          break;
        }
        
        else if(!strcmp(args[i], "|")) {
          ifpipe = 1;
          line_position = i;
        }

        temp[i] = args[i];
      }
      temp[i] = NULL;
      if(ifpara) args[len-1] = ad; // 如果有后台执行，则补回最后一个字符，便于复制到buffer中
      if(redirect) filename = args[i + 1]; // 如果有后台执行，则补回最后一个字符，便于复制到buffer中
    }

    if(ifpipe) {
      for(int i = 0; i < line_position; ++i) {
        father[i] = temp[i];
      }
      for(int i = line_position + 1; i < len; ++i) {
        child[i - line_position - 1] = temp[i];
      }
    }

    // 创建子进程
    pid_t id = fork();

    if(id < 0)
      perror("fork");

    // 父进程
    else if(id) {
      // 后台执行
      if(ifpara) {
        signal(SIGCHLD, handler);
      }
      else {
        int status;
        waitpid(id, &status, 0);
      }
    }

    else {
      // 如果有管道通信，创建新的子进程并行执行两个指令
      if(ifpipe) {
        int pipefd[2];
        if(pipe(pipefd) < 0) {
          printf("Fail to create a pipe!\n");
          should_run = 0;
        }
        pid_t idd = fork();
        if(idd < 0) perror("fork");
        else if(!idd) {
          close(pipefd[0]);
          int fd = dup2(pipefd[1], STDOUT_FILENO);
          if(fd < 0) {
            printf("dup2 error!\n");
            exit(-1);
          }
          execvp(father[0], father);
        }
        else {
          close(pipefd[1]);
          int fd = dup2(pipefd[0], STDIN_FILENO);
          if(fd < 0) {
            printf("dup2 error!\n");
            exit(-1);
          }
          execvp(child[0], child);
        }
      }
      // 如果有重定向输出
      else if(redirect == 1) {
        int oldfd = open(filename, O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
        if(oldfd < 0) {
          printf("Open error!\n");
          exit(-1);
        }

        int fd = dup2(oldfd, STDOUT_FILENO);
        if(fd < 0) {
          printf("dup2 error!\n");
          exit(-1);
        }
        execvp(temp[0], temp);
        close(oldfd);
      }
      // 如果有重定向输入
      else if (redirect == 2) {
        int oldfd = open(filename, O_RDONLY);
        if(oldfd < 0) {
          printf("Open error!\n");
          exit(-1);
        }

        int fd = dup2(oldfd, STDIN_FILENO);
        if(fd < 0) {
          printf("dup2 error!\n");
          exit(-1);
        }
        execvp(temp[0], temp);
        close(oldfd);
      }
      else execvp(temp[0], temp);

      exit(0);
    }

    // 更新buffer
    if(strcmp(args[0], "!!") || args[1]) {
      for(buffer_len = 0; buffer_len <= len; ++buffer_len) {
        if(buffer[buffer_len] != NULL)
          free(buffer[buffer_len]);
        buffer[buffer_len] = args[buffer_len];
      }
      buffer_len--;
    }
    /**
           * After reading user input, the steps are:
           * (1) fork a child process
           * (2) the child process will invoke execvp()
           * (3) if command includes &, parent and child will run concurrently
           */
  }

  return 0;
}
