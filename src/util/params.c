#include <stdio.h>

/**
 * @brief Change parameters in constant.h
 */
int main(int argc, char *argv[]) {
  if(argc < 3) return 0;
  FILE* f = fopen("../include/constant.h", "w");
  fprintf(f, "#define N %s\n#define N_THREADS %s\n#define EST_CLUSTER_SIZE 5", argv[1], argv[2]);
  fclose(f);
  return 0;
}