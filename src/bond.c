#include "../include/bond.h"

/**
 * @param n size of lattice
 * @param p probability of a bond
 * @return Bond* pointer to a bond struct that contains vertical and horizontal bond information
 * 
 * NOTE: this could be parallelised
 */
Bond* bond(float p)
{
  Bond* b = calloc(1, sizeof(Bond));
  if(!b) return NULL;
  b->v = calloc(N*N, sizeof(short));
  b->h = calloc(N*N, sizeof(short));
  if(!b->v || !b->h) return NULL;
  for(int r = 0; r < N; ++r) {
    for(int c = 0; c < N; ++c) {
      if((double)rand()/(double)RAND_MAX < p) b->v[r*N+c] = 1;
      else b->v[r*N+c] = 0; 
      if((double)rand()/(double)RAND_MAX < p) b->h[r*N+c] = 1;
      else b->h[r*N+c] = 0;
      }
  }
  return b;
}

/**
 * @param filename 
 * @brief assume lattice size N
 * @return pointer to a bond struct scanned from a file
 */
Bond* file_bond(char* filename)
{
  Bond* b = calloc(1, sizeof(Bond));
  if(!b) return NULL;
  b->v = calloc(N*N, sizeof(short));
  b->h = calloc(N*N, sizeof(short));
  if(!b->v || !b->h) return NULL;

  int ch, r = 0, c = 0, v_count = 0;
  short gap = 1, vert = 1;
  FILE* f = fopen(filename, "r");
  if(!f) return NULL;
  while((ch = getc(f)) != EOF) {
    if(v_count == N) break; // can have text underneath
    if(vert) {
      if(ch != '\n' && c == N) continue; // skip characters at end of line
      if(ch == ' ') {
        if(gap) gap = 0;
        else {
          gap = 1;
          ++c;
        }
        continue;
      } else if(ch == '\n') {
        vert = 0; c = 0; gap = 0;
      } else if(ch == '|') {
        b->v[r*N+c] = 1;
        gap = 1;
        ++c;
      }
    } else { // in a horizontal line
      if(ch != '\n' && c == N) continue; // skip characters at end of line
      if(ch == '\n') {
        vert = 1; gap = 1;
        c = 0;
        ++r; ++v_count;
      } else if(ch == 'O') {
        gap = 0;
      } else {
        b->h[r*N+c] = (ch == '-') ? 1 : 0;
        ++c;
      }
    }
  }
  return b;
}

void print_bond(Bond* b)
{
  if(N > 40) return;
  int s = (int)log10(N-1) + 1;
  for(int i = 0; i < s; ++i) printf(" ");
  printf(" ");
  for(int c = 0; c < N; ++c) printf("\033[0;34m %*d\033[0;30m", s, c);
  printf("\n\n");
  for(int r = 0; r < N; ++r) {
    printf(" ");
    for(int i = 0; i < s; ++i) printf(" ");
    for(int c = 0; c < N; ++c) {
      for(int i = 0; i < s; ++i) printf(" ");
      printf(b->v[r*N+c] ? "\033[0;31m|\033[0;30m" : " ");
    }
    printf("\033[0;34m\n%*d \033[0;30m", s, r);
    for(int c = 0; c < N; ++c) {
      for(int i = 0; i < s; ++i) printf(b->h[r*N+c] ? "\033[0;31m-\033[0;30m" : " ");
      printf("O");
    }
    printf("\n");
  }
}