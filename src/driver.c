#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  for(int i = 0; i < n; ++i) {
    int pid = fork();
    if(pid == -1) { // error
      break;
    }
    if(pid == 0) { // child
      char *args[] = {"percolate", "100", "0.1", NULL};
      execv("./percolate", args);
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}