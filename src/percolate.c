#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#include "../include/site.h"
#include "../include/bond.h"
#include "../include/cluster.h"
#include "../include/constant.h"

/**
 * @brief Check if a site has at least one bond to a neighbour (and can therefore be the start of a cluster)
 *        Returns true even if neighbour is across the thread boundary (because they need to be joined later).
 * @return short boolean true iff has at least one neighbour
 */
short has_neighbours(Site* a, Bond* b, Site* s)
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
Site* bottom_neighbour(Site* a, Bond* b, Site* s)
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
Site** get_neighbours(Site* a, Bond* b, Site* s, short tid)
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
  Site** nbs = calloc(4, sizeof(Site*));
  if(!nbs) return NULL;
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    // either check site occupation or bond struct
    int start = tid*N*(N/N_THREADS);
    int end = (tid+1)*N*(N/N_THREADS);
    if(tid+1 == N_THREADS) end = N*N;
    if(is[i] < start || is[i] >= end) nbs[i] = NULL; // out of thread bounds
    else if(!nb->seen && (
      (!b && nb->occupied) ||
      (b && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))))
    ) {
      nb->seen = 1; // must mark site here for n=2 case when top neighbour == bottom neighbour
      nbs[i] = nb;
    } else nbs[i] = NULL;
  }
  return nbs;
}

/**
 * @brief loop through neighbours and update cluster
 */
void DFS(Site* a, Bond* b, Site* s, short tid, short* err) {
  Site** nbs = get_neighbours(a, b, s, tid);
  if(!nbs) {
    *err = 1;
    return;
  }
  Cluster* cl = s->cluster;
  for(int i = 0; i < 4; ++i) { // loop through connected, unseen neighbours
    if(*err) return;
    if(!nbs[i]) continue; 
    Site* nb = nbs[i];
    nb->cluster = cl;
    if(!cl->rows[nb->r]) cl->height++;
    if(!cl->cols[nb->c]) cl->width++;
    cl->rows[nb->r] = 1;
    cl->cols[nb->c] = 1;
    cl->size++;
    if(cl->size > EST_CLUSTER_SIZE) {
      cl->sites = realloc(cl->sites, cl->size*sizeof(int)); // grow site index array
      if(!cl->sites) {
        *err = 1;
        return;
      }
    }
    cl->sites[cl->size-1] = nb->r*N+nb->c; // add index of neighbour to cluster's site index array
    DFS(a, b, nb, tid, err);
  }
}

/**
 * @brief Simulate percolation using DFS. Outputs max cluster size and percolation boolean.
 * @param a site array
 * @param b bond struct, either valid address if [-b] or NUll if [-s]
 */
void percolate(Site* a, Bond* b, short tid)
{
  short err = 0;
  int start = tid*N*(N/N_THREADS);
  int end = (tid+1)*N*(N/N_THREADS);
  if(tid+1 == N_THREADS) end = N*N; // last block may be larger
  for(int i = start; i < end; ++i) {
    if(!a[i].seen && ((!b && a[i].occupied) || (b && (has_neighbours(a, b, &a[i]))))) {
      a[i].seen = 1;
      a[i].cluster = cluster(i/N, i%N);
      if(!a[i].cluster) {
        printf("Memory error.\n");
        return;
      }
      DFS(a, b, &a[i], tid, &err);
      if(err) {
        printf("Memory error.\n");
        return;
      }
    }   
  }
}

/**
 * @brief merges clusters along the bottom border of row
 */
void join_row(Site* a, Bond *b, int start, int end, int *err) {
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
    sc->sites = realloc(sc->sites, sc->size);
    if(!sc->sites) {
      *err = 1;
      return;
    }

    // find neighbour's cluster sites and reassign all cluster pointers (inefficient)
    // for(int j = 0; j < N*N; ++j) {
    //   if(!a[j].cluster) continue;
    //   if(a[j].cluster->id == nc->id) a[j].cluster = sc; 
    // }

    // reassign neighbour sites to single cluster
    // int* nc_sites = (int*)calloc(nc_size, sizeof(int)); // make copy of neighbour site index array
    // memcpy(nc_sites, nc->sites, nc_size*sizeof(int));

    for(int j = 0; j < nc->size; ++j) {
      int ix = nc->sites[j];
      sc->sites[j+sc_size-1] = ix;
      if(ix == nb->r*N+nb->c) continue; // don't overwrite neighbour until last
      a[ix].cluster = sc;
    }
    // now overwrite neighbour
    nb->cluster = sc; 
  }
}

/**
 * @brief merges clusters along thread boundaries, and then calculates stats
 * WRONG RESULT WHEN JOINING ALTERNATING CLUSTERS ALONG BORDER
 */
void join_clusters(Site* a, Bond* b) {

  for(int i = 0; i < N_THREADS; ++i) {
    int start = i*N*(N/N_THREADS);
    int end = (i+1)*N*(N/N_THREADS);
    if(i+1 == N_THREADS) end = N*N;
    int err = 0;
    join_row(a, b, start, end, &err);
    if(err) {
      printf("Memory error.\n");
      return;
    }
  }
}

void scan_site_array(Site* a) {
  short perc = 0;
  int max_size = 0;
  // #pragma omp parallel for reduction(max: max_size)
  for(int i = 0; i < N*N; ++i) {
    Cluster *cl = a[i].cluster;
    if(!cl) continue;
    if(cl->size > max_size) max_size = cl->size;
    if(cl->width == N || cl->height == N) perc = 1; 
  }
  printf("Perc: %s\n", perc ? "True" : "False");
  printf(" Max: %d\n", max_size);
}

/**
 * USAGE: ./percolate [-b | -s] [ [-f FILENAME] | PROBABILITY ]
 */
int main(int argc, char *argv[])
{
  clock_t start = clock();

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
      if(!a) {
        printf("Error reading file.\n");
        return 0;
      }
      print_site_array(a);
    } else {
      b = file_bond(fname);
      if(!b) {
        printf("Error reading file.\n");
        return 0;
      }
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
  printf("\nDFS %d-Thread\n%s\n\nN: %d\n", N_THREADS, site ? "Site" : "Bond", N);
  if(p != -1.0) printf("P: %.2f\n", p);
  printf("\n");

  omp_set_num_threads(N_THREADS);
  #pragma omp parallel
  percolate(a, b, omp_get_thread_num());
  if(N_THREADS > 1) join_clusters(a, b);
  scan_site_array(a);
  printf("Time: %.4f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
  return 0;
}