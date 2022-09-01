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

/**
 * @brief Check if a site has at least one bond to a neighbour (and can therefore be the start of a cluster)
 *        Returns true even if neighbour is across the thread boundary (because they need to be joined later).
 * @return short boolean true iff has at least one neighbour
 */
static short has_neighbours(Bond* b, int n, Site* s)
{
  // indices of bonds
  int ib[] = {
    s->r*n+s->c,          // left
    s->r*n+(s->c+n+1)%n,  // right
    s->r*n+s->c,          // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    if((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]])) return 1; 
  }
  return 0;
}

/**
 * @brief get bottom neighbour to site s, intended for across the thread boundary.
 * @return site pointer to bottom neighbour if connected and separate cluster, else NULL
 */
static Site* bottom_neighbour(Site* a, Bond* b, int n, Site* s)
{
  int i = ((s->r+n+1)%n)*n+s->c;
  Site *nb = &a[i];
  if((!b && !nb->occupied) || (b && !b->v[i])) return NULL;
  if(!s->cluster || !nb->cluster) {
    printf("Never.\n");
    return NULL;
  }
  if(s->cluster->id == nb->cluster->id) return NULL;
  return nb;
}

/**
 * @return Site** array of size 4 of neighbour's site pointers. Pointer is either a valid address
 *         if there is a connection to that neighbour, otherwise NULL
 */
static void get_neighbours(Site* a, Bond* b, int n, int n_threads, Site* s, Site* nbs[], short tid)
{
  // indices of neighbours
  int is[] = {
    s->r*n+(s->c+n-1)%n,   // left
    s->r*n+(s->c+n+1)%n,   // right
    ((s->r+n-1)%n)*n+s->c, // top
    ((s->r+n+1)%n)*n+s->c  // bottom
  };
  // indices of bonds
  int ib[] = {
    s->r*n+s->c,          // left
    s->r*n+(s->c+n+1)%n,  // right
    s->r*n+s->c,          // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    int start = tid*n*(n/n_threads);
    int end = (tid+1)*n*(n/n_threads);
    if(tid+1 == n_threads) end = n*n;
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
static void DFS(Site* a, Bond* b, int n, int n_threads, Stack* st, short tid) {
  while(!is_empty(st)) {
    Site *s = pop(st);
    Site *nbs[4];
    get_neighbours(a, b, n, n_threads, s, nbs, tid);
    Cluster* cl = s->cluster;
    for(int i = 0; i < 4; ++i) { // loop through connected, unseen neighbours
      if(!nbs[i]) continue; 
      Site* nb = nbs[i];
      nb->cluster = cl;
      if(!cl->rows[nb->r]) cl->height++;
      if(!cl->cols[nb->c]) cl->width++;
      cl->rows[nb->r] = 1;
      cl->cols[nb->c] = 1;
      cl->sites[cl->size] = nb->r*n+nb->c; // add index of neighbour to cluster's site index array
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
static void percolate(Site* a, Bond* b, int n, int n_threads, CPArray* cpa, short tid)
{
  int start = tid*n*(n/n_threads);
  int end = (tid+1)*n*(n/n_threads);
  if(tid+1 == n_threads) end = n*n; // last block may be larger
  Stack* st = stack(n);
  for(int i = start; i < end; ++i) {
    Site *s = &a[i];
    if(!s->seen && ((!b && s->occupied) || (b && has_neighbours(b, n, s)))) {
      s->seen = 1;
      s->cluster = cluster(n, i/n, i%n);
      cpa->cls[cpa->size++] = s->cluster; 
      add(st, s);
      DFS(a, b, n, n_threads, st, tid);
    }   
  }
  // free_stack(st);
}

/**
 * @brief merges clusters along the bottom border of row
 */
static void join_clusters(Site* a, Bond* b, int n, int n_threads) {
  for(int i = 0; i < n_threads; ++i) {
    int start = i*n*(n/n_threads);
    int end = (i+1)*n*(n/n_threads);
    if(i+1 == n_threads) end = n*n;
    for(int i = start; i < end; ++i) { // loop along row
      Site *s = &a[i];
      if(!s->cluster) continue;
      Site *nb = bottom_neighbour(a, b, n, s);
      if(!nb) continue;
      // else update s->cluster
      Cluster *sc = s->cluster;
      Cluster *nc = nb->cluster;
      if(!nc) { // add single neighbour
        if(!sc->rows[nb->r]) sc->height++;
        sc->rows[nb->r] = 1;
        if(!sc->cols[nb->c]) sc->width++;
        sc->cols[nb->c] = 1;
        sc->sites[sc->size++] = nb->r*n+nb->c;
      }
      else { // combine two clusters
        int sc_size = sc->size;
        sc->size += nc->size;
        for(int i = 0; i < n; ++i) {
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
          if(ix == nb->r*n+nb->c) continue; // don't overwrite neighbour until last
          a[ix].cluster = sc;
        }
        nc->id = -1; // mark as obsolete
      }
      nb->cluster = sc; // now overwrite neighbour
    }
  }
}

static void scan_clusters(CPArray* cpa, int n, int n_threads, int *num, int *max, short *perc) {
  short p = 0;
  int nm = 0, m = 0;
  // #pragma omp parallel for reduction(max: m)
  for(int i = 0; i < n_threads; ++i) {
    for(int j = 0; j < cpa[i].size; ++j) {
      Cluster *cl = cpa[i].cls[j];
      if(cl->id == -1) continue;
      nm++;
      if(cl->size > m) m = cl->size;
      if(p) continue;
      if(cl->width == n || cl->height == n) p = 1;
      // free_cluster(cl);
    }
  }
  *perc = p;
  *num = nm;
  *max = m;
  // free_cparray(cpa, n_threads);
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

  int n;
  float p = -1.0;
  int n_threads = 1;
  
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
    if(argc - optind < 1) {
      printf("Invalid arguments.\n");
      return 0;
    }
    n = atoi(argv[optind++]);
    if(argc - optind > 0) n_threads = atoi(argv[optind]);
    if(site) {
      a = file_site_array(fname, n);
      if(!a) {
        printf("Memory error.\n");
        return 0;
      }
      print_site_array(a, n);
    } else {
      b = file_bond(fname, n);
      if(!b) {
        printf("Memory error.\n");
        return 0;
      }
      a = site_array(n, -1.0);
      if(!a) {
        printf("Memory error.\n");
        return 0;
      }
      print_bond(b, n);
    }
  }
  else {
    if(argc - optind < 2) {
      printf("Invalid arguments.\n");
      return 0;
    }
    n = atoi(argv[optind++]);
    p = atof(argv[optind++]);
    if(argc - optind > 0) n_threads = atoi(argv[optind]);

    if(p > 1.0) p = 1.0;
    srand(time(NULL));
    if(site) {
      a = site_array(n, p);
      print_site_array(a, n);
    } else {
      b = bond(n, p);
      a = site_array(n, -1.0);
      print_bond(b, n);
    }
  }
  if(n < 1 || n_threads < 1) {
    printf("Invalid arguments.");
    return 0;
  }
  int max_clusters = n % 2 == 0 ? n*n/2 : (n-1)*(n-1)/2+1;

  int max_threads = omp_get_max_threads();
  if(n_threads > max_threads) n_threads = max_threads;
  if(n_threads > n) n_threads = n;
  omp_set_num_threads(n_threads);
  CPArray* cpa = cluster_array(n_threads, max_clusters); // each thread keeps an array of its cluster pointers 

  printf("\n%s %d-Thread\n\nN: %d\n", site ? "Site" : "Bond", n_threads, n);
  if(p != -1.0) printf("P: %.2f\n", p);  

  clock_t init = clock();
  double init_o = omp_get_wtime();
  printf("\n Init time: %9.6f %9.6f\n", (double)(init-start)/CLOCKS_PER_SEC, init_o-start_o);

  #pragma omp parallel
  {
    int num = omp_get_thread_num();
    percolate(a, b, n, n_threads, &cpa[num], num);
  }
  clock_t perc_t = clock();
  double perc_o = omp_get_wtime();
  printf(" Perc time: %9.6f %9.6f\n", (double)(perc_t-init)/CLOCKS_PER_SEC, perc_o-init_o);

  if(n_threads > 1) join_clusters(a, b, n, n_threads);
  clock_t join = clock();
  double join_o = omp_get_wtime();
  printf(" Join time: %9.6f %9.6f\n", (double)(join-perc_t)/CLOCKS_PER_SEC, join_o-perc_o);
  
  // free(a);
  // if(b) free_bond(b);

  int num = 0, max = 0;
  short perc = 0;
  scan_clusters(cpa, n, n_threads, &num, &max, &perc);
  printf(" Scan time: %9.6f %9.6f\n", (double)(clock()-join)/CLOCKS_PER_SEC, omp_get_wtime()-join_o);

  printf("Total time: %9.6f %9.6f\n", (double)(clock()-start)/CLOCKS_PER_SEC, omp_get_wtime()-start_o);
  printf("\n Num: %d\n Max: %d\nPerc: %s\n\n", num, max, perc ? "True" : "False");
  return 0;
}