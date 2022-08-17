#include "site.h"
#include <stdio.h>
#include <math.h>

/**
 * @param n size of square lattice
 * @param p probability of site occupancy
 * @return short* pointer to site array
 */
Site* site_array(int n, float p)
{
  Site* sites = calloc(n*n, sizeof(Site));
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      sites[r*n+c].r = r;
      sites[r*n+c].c = c;
      sites[r*n+c].size = malloc(sizeof(int));
      *(sites[r*n+c].size) = 1;
      // don't allocate memory for rows, cols unless required
      if((double)rand()/(double)RAND_MAX < p) sites[r*n+c].occupied = 1;
      else sites[r*n+c].occupied = 0; 
    }
  }
  return sites;
}

Site* file_site_array(char* filename, int* n) {
  // scan first line to find n
  FILE* f = fopen(filename, "r");
  if(!f) return NULL;
  int ch;
  *n = 0;
  while((ch = getc(f)) != '\n') {
    if(ch != ' ') ++(*n);
  }
  fseek(f, 0, SEEK_SET);

  Site* s = calloc((*n)*(*n), sizeof(Site));

  int i = 0, r = 0, c = 0;
  while((ch = getc(f)) != EOF) {
    if(ch == ' ') continue;
    else if(ch == '\n') {
      c = 0;
      ++r;
      continue;
    }
    s[i].r = r;
    s[i].c = c;
    s[i].size = malloc(sizeof(int));
    *(s[i].size) = 1;

    if(ch == 'X') s[i].occupied = 1;
    else s[i].occupied = 0;
    ++c; ++i;
  }
  fclose(f);
  return s;
}

void free_site_array(Site* s, int n) {
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
  int s = (int)log10(n) + 1;
  printf(" ");
  for(int i = 0; i < s; ++i) printf(" ");
  for(int c = 0; c < n; ++c) printf("\033[0;34m %*d\033[0;30m", s, c);
  printf("\n\n");
  for(int r = 0; r < n; ++r) {
    printf("\033[0;34m%*d \033[0;30m", s, r);
    for(int c = 0; c < n; ++c) {
      for(int i = 0; i < s; ++i) printf(" ");
      if(a[r*n+c].occupied) {
        printf("\033[0;31mX\033[0;30m");
      }
      else printf("O");
    }
    printf("\n");
  }
  printf("\n");
}