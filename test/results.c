#include <stdlib.h>
#include <stdio.h>

#define NUM_RESULTS 10
#define NUM_DATA_POINTS 10
#define DATA_POINT_SIZE 10

float** read_file(char *fname, int *size) {
  int s = NUM_RESULTS; // current allocated memory
  float **results = malloc(s * sizeof(float*));
  int r = 0; // current result number
  results[r] = malloc(NUM_DATA_POINTS * sizeof(float));

  int c;
  int n = 0; // data point number in result
  int i = 0; // character index in single data point
  char tmp[DATA_POINT_SIZE]; // read float from this buffer

  FILE *f = fopen(fname, "r");

  while((c = fgetc(f)) != EOF) {
    if(c == ',' || c == '\n') {
      tmp[i] = '\0';
      i = 0;
      results[r][n++] = atof(tmp);
      if(c == '\n') {
        if(r == s-1) {
          s += NUM_RESULTS;
          results = realloc(results, s * sizeof(float*));
        }
        results[++r] = malloc(NUM_DATA_POINTS * sizeof(float));
        n = 0;
      }
    }
    else tmp[i++] = (char)c;
  }
  *size = r; // account for newline at EOF
  return results;
}

void print_results(float **results, int size) {
  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < NUM_DATA_POINTS; ++j) {
      printf("%f ", results[i][j]);
    }
    printf("\n");
  }
}

float* get_avgs(float **results, int size, int n, float p, int n_threads) {
  float *avgs = calloc(NUM_DATA_POINTS-3, sizeof(float));
  int num = 0;
  for(int i = 0; i < size; ++i) {
    if(results[i][0] == n && results[i][1] == p && results[i][2] == n_threads) {
      for(int j = 0; j < NUM_DATA_POINTS-3; ++j) avgs[j] += results[i][j+3];
      ++num;
    }
  }
  for(int i = 0; i < NUM_DATA_POINTS-3; ++i) avgs[i] /= num;
  return avgs;
}

/**
 * @brief query results file to get average results for a set of parameters (n p n_threads)
 *        ./results results_file n p n_threads 
 */
int main(int argc, char *argv[]) {
  if(argc < 5) exit(EXIT_FAILURE);
  
  int size;
  float **results = read_file(argv[1], &size);
  
  int n = atoi(argv[2]);
  float p = atof(argv[3]);
  int n_threads = atoi(argv[4]);

  float *avgs = get_avgs(results, size, n, p, n_threads);
  for(int i = 0; i < NUM_DATA_POINTS-3; ++i) printf("%f ", avgs[i]);
  printf("\n");

  exit(EXIT_SUCCESS);
}