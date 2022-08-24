#include "../include/site.h"

/**
 * @param p probability of occupation. Negative p will skip the occupation selection step
 * @return Site* pointer to site array
 * 
 * NOTE: this could be parallelised
 */
Site* site_array(float p)
{
  Site* sites = calloc(N*N, sizeof(Site));
  if(!sites) return NULL;
  for(int i = 0; i < N*N; ++i) {
    sites[i].r = i/N;
    sites[i].c = i%N;
    if(p > 0) {
      if((double)rand()/(double)RAND_MAX < p) sites[i].occupied = 1;
      else sites[i].occupied = 0; 
    }
    // MAYBE: sites[i].cluster = (Cluster**)calloc(1, sizeof(Cluster*));
  }
  return sites;
}

/**
 * @brief assume lattice size N
 * @param filename 
 * @return Site* site array scanned from file
 */
Site* file_site_array(char* filename) {
  Site* s = calloc(N*N, sizeof(Site));
  if(!s) return NULL;

  int ch, r = 0, c = 0;
  FILE* f = fopen(filename, "r");
  if(!f) return NULL;
  while((ch = getc(f)) != EOF) {
    if(ch == ' ' || (c == N && ch != '\n')) continue;
    else if(ch == '\n') {
      c = 0;
      ++r;
      if(r == N) break; // can have text underneath
      continue;
    }
    s[r*N+c].r = r;
    s[r*N+c].c = c;

    if(ch == 'X') s[r*N+c].occupied = 1;
    else s[r*N+c].occupied = 0;
    ++c;
  }
  fclose(f);
  return s;
}

void free_site_array(Site* a)
{
  for(int i = 0; i < N*N; ++i) {
    if(a[i].cluster) free_cluster(a[i].cluster);
  }
  free(a);
}

void print_site_array(Site* a)
{
  if(N > 40) return;
  int s = (int)log10(N-1) + 1;
  printf(" ");
  for(int i = 0; i < s; ++i) printf(" ");
  for(int c = 0; c < N; ++c) printf("\033[0;34m %*d\033[0;30m", s, c);
  printf("\n\n");
  for(int r = 0; r < N; ++r) {
    printf("\033[0;34m%*d \033[0;30m", s, r);
    for(int c = 0; c < N; ++c) {
      for(int i = 0; i < s; ++i) printf(" ");
      if(a[r*N+c].occupied) printf("\033[0;31mX\033[0;30m");
      else printf("O");
    }
    printf("\n");
  }
}