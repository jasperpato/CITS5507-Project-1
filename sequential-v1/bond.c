#include "bond.h"

Bond* bond(int n, float p)
{
  Bond* b = calloc(1, sizeof(Bond));
  b->v = calloc(n*n, sizeof(short));
  b->h = calloc(n*n, sizeof(short));
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      if((double)rand()/(double)RAND_MAX < p) b->v[r*n+c] = 1;
      else b->v[r*n+c] = 0; 
      if((double)rand()/(double)RAND_MAX < p) b->h[r*n+c] = 1;
      else b->h[r*n+c] = 0;
      }
  }
  return b;
}

void free_bond(Bond* b)
{
  free(b->v);
  free(b->h);
  free(b);
}

void print_bond(Bond* b, int n) {
  if(n > 40) return;
  int s = (int)log10(n-1) + 1;
  for(int i = 0; i < s; ++i) printf(" ");
  printf(" ");
  for(int c = 0; c < n; ++c) printf("\033[0;34m %*d\033[0;30m", s, c);
  printf("\n\n");
  for(int r = 0; r < n; ++r) {
    printf(" ");
    for(int i = 0; i < s; ++i) printf(" ");
    for(int c = 0; c < n; ++c) {
      for(int i = 0; i < s; ++i) printf(" ");
      printf(b->v[r*n+c] ? "\033[0;31m|\033[0;30m" : " ");
    }
    printf("\033[0;34m\n%*d \033[0;30m", s, r);
    for(int c = 0; c < n; ++c) {
      for(int i = 0; i < s; ++i) printf(b->h[r*n+c] ? "\033[0;31m-\033[0;30m" : " ");
      printf("O");
    }
    printf("\n");
  }
}