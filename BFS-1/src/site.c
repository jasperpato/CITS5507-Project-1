#include "../include/site.h"

/**
 * @param n size of 2D site array
 * @param p probability of occupation. Negative p will skip the occupation selection step
 * @return Site* pointer to site array
 * 
 * NOTE: this could be parallelised
 */
Site* site_array(int n, float p)
{
  clock_t start = clock();
  Site* sites = calloc(n*n, sizeof(Site));
  if(!sites) return NULL;
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      sites[r*n+c].r = r;
      sites[r*n+c].c = c;
      sites[r*n+c].size = calloc(1, sizeof(int));
      if(!sites[r*n+c].size) return NULL;
      *(sites[r*n+c].size) = 1;
      // NOTE: not allocating memory for rows, cols until required
      if(p > 0) {
        if((double)rand()/(double)RAND_MAX < p) sites[r*n+c].occupied = 1;
        else sites[r*n+c].occupied = 0; 
      }
    }
  }
  return sites;
}

/**
 * @param filename 
 * @param n pointer to lattice size variable to write
 * @return Site* site array scanned from file
 */
Site* file_site_array(char* filename, int* n) {
  // scan first line to find n
  FILE* f = fopen(filename, "r");
  if(!f) return NULL;
  int ch;
  *n = 0;
  while((ch = getc(f)) != '\n') {
    if(ch == 'X' || ch == 'O') ++(*n);
  }
  fseek(f, 0, SEEK_SET);

  Site* s = calloc((*n)*(*n), sizeof(Site));
  if(!s) return NULL;

  int r = 0, c = 0;
  while((ch = getc(f)) != EOF) {
    if(ch == ' ' || (c == *n && ch != '\n')) continue;
    else if(ch == '\n') {
      c = 0;
      ++r;
      if(r == *n) break; // can have text underneath
      continue;
    }
    s[r*(*n)+c].r = r;
    s[r*(*n)+c].c = c;
    s[r*(*n)+c].size = calloc(1, sizeof(int));
    if(!s[r*(*n)+c].size) return NULL;
    *(s[r*(*n)+c].size) = 1;

    if(ch == 'X') s[r*(*n)+c].occupied = 1;
    else s[r*(*n)+c].occupied = 0;
    ++c;
  }
  fclose(f);
  return s;
}

void free_site_array(Site* s, int n)
{
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      free(s[r*n+c].rows);
      free(s[r*n+c].cols);
    }
  }
  free(s);
}

void print_site_array(Site* a, int n)
{
  if(n > 40) return;
  int s = (int)log10(n-1) + 1;
  printf(" ");
  for(int i = 0; i < s; ++i) printf(" ");
  for(int c = 0; c < n; ++c) printf("\033[0;34m %*d\033[0;30m", s, c);
  printf("\n\n");
  for(int r = 0; r < n; ++r) {
    printf("\033[0;34m%*d \033[0;30m", s, r);
    for(int c = 0; c < n; ++c) {
      for(int i = 0; i < s; ++i) printf(" ");
      if(a[r*n+c].occupied) printf("\033[0;31mX\033[0;30m");
      else printf("O");
    }
    printf("\n");
  }
}