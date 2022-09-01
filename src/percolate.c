#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#include "../include/stack.h"
#include "../include/site.h"
#include "../include/bond.h"
#include "../include/cluster.h"

#include "../include/constant.h"

/**
 * @brief Check if a site has at least one bond to a neighbour (and can therefore be the start of a cluster)
 *        Returns true even if neighbour is across the thread boundary (because they need to be joined later).
 * @return short boolean true iff has at least one neighbour
 */
static short has_neighbours(Site* a, Bond* b, Site* s)
{
  // indices of neighbours
  int is[] = {
    s->r*N+(s->c+N-1)%N,   // left
    s->r*N+(s->c+N+1)%N,   // right
    ((s->r+N-1)%N)*N+s->c, // top
    ((s->r+N+1)%N)*N+s->c  // bottom
  };
  // indices of bonds
  int ib[] = {
    s->r*N+s->c,          // left
    s->r*N+(s->c+N+1)%N,  // right
    s->r*N+s->c,          // top
    ((s->r+N+1)%N)*N+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    if(!nb->seen && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))) return 1;
  }
  return 0;
}

/**
 * @brief get bottom neighbour to site s, intended for across the thread boundary.
 * @return site pointer to bottom neighbour if connected and separate cluster, else NULL
 */
static Site* bottom_neighbour(Site* a, Bond* b, Site* s)
{
  int i = ((s->r+N+1)%N)*N+s->c;
  Site *nb = &a[i];
  if((!b && !nb->occupied) || (b && !b->v[i])) return NULL;
  if(!nb->cluster || !s->cluster) return NULL;
  if(s->cluster->id == nb->cluster->id) return NULL;
  return nb;
}

/**
 * @return Site** array of size 4 of neighbour's site pointers. Pointer is either a valid address
 *         if there is a connection to that neighbour, otherwise NULL
 */
static void get_neighbours(Site* a, Bond* b, Site* s, Site* nbs[], short tid)
{
  // indices of neighbours
  int is[] = {
    s->r*N+(s->c+N-1)%N,   // left
    s->r*N+(s->c+N+1)%N,   // right
    ((s->r+N-1)%N)*N+s->c, // top
    ((s->r+N+1)%N)*N+s->c  // bottom
  };
  // indices of bonds
  int ib[] = {
    s->r*N+s->c,          // left
    s->r*N+(s->c+N+1)%N,  // right
    s->r*N+s->c,          // top
    ((s->r+N+1)%N)*N+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    int start = tid*N*(N/N_THREADS);
    int end = (tid+1)*N*(N/N_THREADS);
    if(tid+1 == N_THREADS) end = NUM_SITES;
    if(is[i] < start || is[i] >= end) nbs[i] = NULL; // out of thread bounds
    else if(!nb->seen && (
      (!b && nb->occupied) ||
      (b && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))))
    ) {
      nb->seen = 1; // must mark site here for n=2 case when top neighbour == bottom neighbour
      nbs[i] = nb;
    } else nbs[i] = NULL;
  }
}

/**
 * @brief loop through neighbours and update cluster
 */
static void DFS(Site* a, Bond* b, Stack* st, short tid) {
  while(!is_empty(st)) {
    Site *s = pop(st);
    Site *nbs[4];
    get_neighbours(a, b, s, nbs, tid);
    Cluster* cl = s->cluster;
    for(int i = 0; i < 4; ++i) { // loop through connected, unseen neighbours
      if(!nbs[i]) continue; 
      Site* nb = nbs[i];
      nb->cluster = cl;
      if(!cl->rows[nb->r]) cl->height++;
      if(!cl->cols[nb->c]) cl->width++;
      cl->rows[nb->r] = 1;
      cl->cols[nb->c] = 1;
      cl->sites[cl->size] = nb->r*N+nb->c; // add index of neighbour to cluster's site index array
      cl->size++;
      add(st, nb);
    }
  }
}

/**
 * @brief Simulate percolation using DFS. Outputs max cluster size and percolation boolean.
 * @param a site array
 * @param b bond struct, either valid address if [-b] or NUll if [-s]
 */
static void percolate(Site* a, Bond* b, CPArray* cpa, short tid)
{
  int start = tid*N*(N/N_THREADS);
  int end = (tid+1)*N*(N/N_THREADS);
  if(tid+1 == N_THREADS) end = NUM_SITES; // last block may be larger
  Stack* st = stack();
  for(int i = start; i < end; ++i) {
    Site *s = &a[i];
    if(!s->seen && ((!b && s->occupied) || (b && (has_neighbours(a, b, s))))) {
      s->seen = 1;
      s->cluster = cluster(i/N, i%N);
      cpa->cls[cpa->size++] = s->cluster; 
      add(st, s);
      DFS(a, b, st, tid);
    }   
  }
  free_stack(st);
}

/**
 * @brief merges clusters along the bottom border of row
 */
static void join_clusters(Site* a, Bond* b) {
  for(int i = 0; i < N_THREADS; ++i) {
    int start = i*N*(N/N_THREADS);
    int end = (i+1)*N*(N/N_THREADS);
    if(i+1 == N_THREADS) end = NUM_SITES;
    for(int i = start; i < end; ++i) { // loop along row
      Site *s = &a[i];
      Site *nb = bottom_neighbour(a, b, s);
      if(!nb) continue;
      // else update s->cluster
      Cluster *sc = s->cluster;
      Cluster *nc = nb->cluster;
      int sc_size = sc->size; // , nc_size = nc->size;
      sc->size += nc->size;
      for(int i = 0; i < N; ++i) {
        if(nc->rows[i]) {
          if(!sc->rows[i]) sc->height++;
          sc->rows[i] = 1;
        }
        if(nc->cols[i]) {
          if(!sc->cols[i]) sc->width++;
          sc->cols[i] = 1;
        }
      }
      for(int j = 0; j < nc->size; ++j) {
        int ix = nc->sites[j];
        sc->sites[j+sc_size] = ix;
        if(ix == nb->r*N+nb->c) continue; // don't overwrite neighbour until last
        a[ix].cluster = sc;
      }
      // now overwrite neighbour
      nb->cluster = sc; 
    }
  }
}

/**
 * @brief loop through each site and scan cluster data
 */
static void scan_site_array(Site* a, short *perc, int *max) {
  short p = 0;
  int m = 0;
  // #pragma omp parallel for reduction(max: m)
  for(int i = 0; i < NUM_SITES; ++i) {
    Cluster *cl = a[i].cluster;
    if(!cl) continue;
    if(cl->size > m) m = cl->size;
    if(p) continue;
    if(cl->width == N || cl->height == N) p = 1;
  }
  *perc = p;
  *max = m;
}

/**
 * USAGE: ./percolate [-b | -s] [ [-f FILENAME] | PROBABILITY ]
 */
int main(int argc, char *argv[])
{
  clock_t start = clock();
  double start_o = omp_get_wtime();
  Site* a = NULL;
  Bond* b = NULL;
  short site = 1;
  char* fname = NULL;
  float p = -1.0;
  int c;
  while ((c = getopt(argc, argv, "sbf:")) != -1) {
    if(c == 'b') site = 0;
    else if(c == 'f') {
      if(!optarg) {
        printf("Error reading file.\n");
        return 0;
      }
      fname = optarg;
    }
  }
  if(fname) {
    if(site) {
      a = file_site_array(fname);
      print_site_array(a);
    } else {
      b = file_bond(fname);
      a = site_array(-1.0);
      print_bond(b);
    }
  }
  else {
    if(argc - optind < 1) {
      printf("Invalid arguments.\n");
      return 0;
    }
    p = atof(argv[optind]);
    if(p > 1.0) p = 1.0;
    srand(time(NULL));
    if(site) {
      a = site_array(p);
      print_site_array(a);
    } else {
      b = bond(p);
      a = site_array(-1.0);
      print_bond(b);
    }
  }
  printf("\n%s %d-Thread\n\nN: %d\n", site ? "Site" : "Bond", N_THREADS, N);
  if(p != -1.0) printf("P: %.2f\n", p);
  // each thread keeps an array of its cluster pointers 

  CPArray* cpa = cluster_array();
  omp_set_num_threads(N_THREADS);
  clock_t init = clock();
  double init_o = omp_get_wtime();
  printf("\n Init time: %9.6f %9.6f\n", (double)(init-start)/CLOCKS_PER_SEC, init_o-start_o);

  #pragma omp parallel
  {
    int num = omp_get_thread_num();
    percolate(a, b, &cpa[num], num);
  }
  clock_t perc_t = clock();
  double perc_o = omp_get_wtime();
  printf(" Perc time: %9.6f %9.6f\n", (double)(perc_t-init)/CLOCKS_PER_SEC, perc_o-init_o);

  if(N_THREADS > 1) join_clusters(a, b);
  clock_t join = clock();
  double join_o = omp_get_wtime();
  printf(" Join time: %9.6f %9.6f\n", (double)(join-perc_t)/CLOCKS_PER_SEC, join_o-perc_o);
  
  free(a);
  if(b) free_bond(b);

  short perc = 0;
  int max = 0;
  // scan_site_array(a, &perc, &max);
  scan_clusters(cpa, &perc, &max);
  printf(" Scan time: %9.6f %9.6f\n", (double)(clock()-join)/CLOCKS_PER_SEC, omp_get_wtime()-join_o);

  printf("Total time: %9.6f %9.6f\n", (double)(clock()-start)/CLOCKS_PER_SEC, omp_get_wtime()-start_o);
  printf("\nPerc: %s\n Max: %d\n\n", perc ? "True" : "False", max);
  return 0;
}