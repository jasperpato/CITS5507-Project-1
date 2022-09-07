#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define N_MIN 100
#define N_MAX 3000
#define N_STEP 100

#define P_MIN 0
#define P_MAX 1.0
#define P_STEP 0.1

#define P_RES (int)1e3

#define NT_MIN 1
#define NT_MAX 4
#define NT_STEP 1

#define ARG_LENGTH 40

/**
 * USAGE: ./driver RESULTS_FILENAME
 * 
 * Loops through n, p and n_threads values and calls percolate infinitely
 */
int main(int argc, char *argv[]) {
  srand(time(NULL));
  short err = 0;
  while(1) {
    for(int n = N_MIN; n <= N_MAX; n+=N_STEP) {
      if(err) break;
      for(int pi = (int)(P_MIN*P_RES); pi <= (int)(P_MAX*P_RES); pi+=(int)(P_STEP*P_RES)) {
        float p = (float)pi/P_RES;
        int seed = rand();
        if(err) break;
        for(int nt = NT_MIN; nt <= NT_MAX; nt+=NT_STEP) {
          int pid = fork();
          if(pid == -1) {
            err = 1;
            break;
          }
          else if(pid == 0) { // child
            char *args[] = {"percolate", "-v", "-p", NULL, "-r", NULL, NULL, NULL, NULL, NULL};
            args[3] = malloc(ARG_LENGTH*sizeof(char));
            for(int i = 5; i < 9; ++i) args[i] = malloc(ARG_LENGTH*sizeof(char));
            sprintf(args[3], "%s", argv[1]);
            sprintf(args[5], "%d", seed); // same seed for all n_thread values
            sprintf(args[6], "%d", n);
            sprintf(args[7], "%f", p);
            sprintf(args[8], "%d", nt);
            execv("../src/percolate", args);
            exit(EXIT_FAILURE);
          }
          else {
            int status;
            wait(&status);
            // sleep(1); // if n < 500ish, wait for new second for srand()
          }
        }
      }
    }
  }
  // exit(EXIT_SUCCESS);
}