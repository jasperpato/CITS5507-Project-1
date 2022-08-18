#include "bond.h"

Bond* bond(int n, float p)
{
  Bond* b = malloc(sizeof(Bond));
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

Bond* file_bond(char* filename, int* n)
{
  // scan second line to find n
  FILE* f = fopen(filename, "r");
  if(!f) return NULL;
  int ch;
  *n = 0;
  while((ch = getc(f)) != '\n') ; // skip first line of vertical bonds
  getc(f); // skip new line
  while((ch = getc(f)) != '\n' && ch != EOF) {
    if(ch == 'O') ++(*n);
  }
  fseek(f, 0, SEEK_SET);

  Bond* b = malloc(sizeof(Bond));
  b->v = calloc((*n)*(*n), sizeof(short));
  b->h = calloc((*n)*(*n), sizeof(short));

  int r  = 0, c = 0, v_count = 0, h_count = 0;
  short gap = 1, vert = 1;
  while((ch = getc(f)) != EOF) {
    if(v_count == *n) break;
    if(vert) {
      if(ch != '\n' && h_count == *n) continue; // skip characters at end of line
      if(ch == ' ') {
        if(gap) gap = 0;
        else {
          gap = 1;
          ++c; ++h_count;
        }
        continue;
      } else if(ch == '\n') {
        vert = 0; c = 0; gap = 0; h_count = 0;
      } else if(ch == '|') {
        b->v[r*(*n)+c] = 1;
        gap = 1;
        ++c; ++h_count;
      }
    } else { // in a horizontal line
      if(ch != '\n' && h_count == *n) continue; // skip characters at end of line
      if(ch == '\n') {
        vert = 1; gap = 1;
        h_count = 0; c = 0;
        ++r; ++v_count;
      } else if(ch == 'O') {
        gap = 0;
        ++h_count;
      } else {
        b->h[r*(*n)+c] = (ch == '-') ? 1 : 0;
        ++c;
      }
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

void print_bond(Bond* b, int n)
{
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