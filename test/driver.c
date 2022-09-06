#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N_MIN 200
#define N_MAX 5000
#define N_STEP 200

#define P_MIN 0
#define P_MAX 1.0
#define P_STEP 0.1

#define NT_MIN 1
#define NT_MAX 4
#define NT_STEP 1

int main(int argc, char *argv[]) {
  short err = 0;
  while(1) {
    for(int n = N_MIN; n <= N_MAX; n+=N_STEP) {
      if(err) break;
      for(int pi = (int)(P_MIN*100); pi <= (int)(P_MAX*100); pi+=(int)(P_STEP*100)) {
        float p = (float)pi/100.0;
        if(err) break;
        for(int nt = NT_MIN; nt <= NT_MAX; nt+=NT_STEP) {
          int pid = fork();
          if(pid == -1) {
            err = 1;
            break;
          }
          else if(pid == 0) { // child
            char *args[] = {"percolate", "-v", "-p", NULL, NULL, NULL, NULL, NULL};
            for(int i = 3; i < 7; ++i) args[i] = malloc(50*sizeof(char));
            sprintf(args[3], "%s", argv[1]);
            sprintf(args[4], "%d", n);
            sprintf(args[5], "%.2f", p);
            sprintf(args[6], "%d", nt);
            execv("../src/percolate", args); // change prog name and pipe command line arguments to percolate
            exit(EXIT_FAILURE);
          }
          else {
            int status;
            wait(&status); // wait for child
            // sleep(1); // if n < 500ish, wait for new second for srand()
          }
        }
      }
    }
    break;
  }
  exit(EXIT_SUCCESS);
}