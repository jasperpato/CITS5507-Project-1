#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 100

/**
 * @brief Calls percolate N times. Should use -v and -p to silence printing and store results in a file.
 *        ./driver -v -p results_file n p n_threads
 */
int main(int argc, char *argv[]) {
  for(int i = 0; i < N; ++i) {
    int pid = fork();
    if(pid == -1) break;
    else if(pid == 0) { // child
      argv[0] = "percolate";
      execv("./percolate", argv); // change prog name and pipe command line arguments to percolate
      exit(EXIT_FAILURE);
    }
    else {
      int status;
      wait(&status); // wait for child
      // sleep(1); // if n < 500ish, wait for new second for srand()
    }
  }
  exit(EXIT_SUCCESS);
}