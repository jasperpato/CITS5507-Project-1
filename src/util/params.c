#include <stdio.h>

/**
 * @brief Change parameters in constant.h
 */
int main(int argc, char *argv[]) {
  if(argc < 3) return 0;
  FILE* f = fopen("../include/constant.h", "w");
  fprintf(f, "#define N %s\n#define NUM_SITES N*N\n#define MAX_CLUSTERS N % 2 == 0: NUM_SITES/2 : (N-1)*(N-1)/2+1\n#define N_THREADS %s", argv[1], argv[2]);
  fclose(f);
  return 0;
}