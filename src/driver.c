#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 100

int main(int argc, char *argv[]) {
  for(int i = 0; i < N; ++i) {
    int pid = fork();
    if(pid == -1) break;
    else if(pid == 0) { // child
      argv[0] = "percolate";
      execv("./percolate", argv);
      exit(EXIT_FAILURE);
    }
    else {
      int status;
      wait(&status);
      // sleep(1);
    }
  }
  exit(EXIT_SUCCESS);
}