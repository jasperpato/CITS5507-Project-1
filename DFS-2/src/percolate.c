#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#include "../include/site.h"
#include "../include/bond.h"
#include "../include/cluster.h"

/**
 * @brief Check if a site has at least one bond to a neighbour (and can therefore be the start of a cluster)
 *        Returns true even if neighbour is across the thread boundary (because they need to be joined later).
 * @return short boolean true iff has at least one neighbour
 */
short has_neighbours(Site* a, Bond* b, int n, Site* s)
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
    if(!nb->seen && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))) return 1;
  }
  return 0;
}

/**
 * @brief get bottom neighbour to site s, across the thread boundary.
 * @return site pointer to bottom neighbour if connected and separate cluster, else NULL
 */
Site* bottom_neighbour(Site* a, Bond* b, int n, Site* s)
{
  int i = ((s->r+n+1)%n)*n+s->c;
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
Site** get_neighbours(Site* a, Bond* b, int n, Site* s, short t)
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
  Site** nbs = calloc(4, sizeof(Site*));
  if(!nbs) return NULL;
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    // either check site occupation or bond struct
    if((!t && is[i] >= n*n/2) || (t && is[i] < n*n/2)) nbs[i] = NULL; // out of thread bounds
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
void DFS(Site* a, Bond* b, int n, Site* s, short t, short* err) {
  Site** nbs = get_neighbours(a, b, n, s, t);
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
    cl->size++;
    if(!cl->rows[nb->r]) cl->height++;
    if(!cl->cols[nb->c]) cl->width++;
    cl->rows[nb->r] = 1;
    cl->cols[nb->c] = 1;
    DFS(a, b, n, nb, t, err);
  }
}

/**
 * @brief Simulate percolation using DFS. Outputs max cluster size and percolation boolean.
 * @param a site array
 * @param n size of site array
 * @param b bond struct, either valid address if [-b] or NUll if [-s]
 */
void percolate(Site* a, Bond* b, int n, short t)
{
  short err = 0;
  for(int i = t*n*(n/2); i < (t+1)*n*(n/2); ++i) {
    if(!a[i].seen && ((!b && a[i].occupied) || (b && (has_neighbours(a, b, n, &a[i]))))) {
      a[i].seen = 1;
      a[i].cluster = cluster(i/n, i%n, n);
      if(!a[i].cluster) {
        printf("Memory error.\n");
        return;
      }
      DFS(a, b, n, &a[i], t, &err);
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
void join_row(Site* a, Bond *b, int n, int start, int end) {
  for(int i = start; i < end; ++i) { // loop along row
    Site *s = &a[i];
    Site *nb = bottom_neighbour(a, b, n, s);
    if(!nb) continue;
    // else update s->cluster
    Cluster *sc = s->cluster;
    Cluster *nc = nb->cluster;
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
    copy_cluster(sc, nc); // overwrite neighbour cluster with s->cluster
  }
}

/**
 * @brief merges clusters along thread boundaries, and then calculates stats
 */
void join_clusters(Site* a, Bond* b, int n) {

  join_row(a, b, n, n*(n/2-1), n*(n/2));
  join_row(a, b, n, n*(n/2), n*n);

  short perc = 0;
  int max_size = 0;
  #pragma omp parallel for reduction(max: max_size)
  for(int i = 0; i < n*n; ++i) {
    Cluster *cl = a[i].cluster;
    if(!cl) continue;
    if(cl->size > max_size) max_size = cl->size;
    if(cl->width == n || cl->height == n) perc = 1; 
  }

  printf("Perc: %s\n", perc ? "True" : "False");
  printf(" Max: %d\n", max_size);
}

/**
 * USAGE: ./percolate [-b | -s] [-t] [[-f FILENAME] | [LATTICE_SIZE PROBABILITY]]
 */
int main(int argc, char *argv[])
{
  clock_t start = clock();

  Site* a = NULL;
  Bond* b = NULL;
  int n = 0;
  float p = -1.0;
  short site = 1;
  char* fname = NULL;

  int c;
  while ((c = getopt(argc, argv, "sbf:")) != -1) {
    if(c == 'b') site = 0;
    else if(c == 'f') {
      if(!optarg) return 0;
      fname = optarg;
    }
  }
  if(fname) {
    if(site) {
      a = file_site_array(fname, &n);
      if(!a) {
        printf("Error reading file.\n");
        return 0;
      }
      print_site_array(a, n);
    } else {
      b = file_bond(fname, &n);
      if(!b) {
        printf("Error reading file.\n");
        return 0;
      }
      a = site_array(n, -1.0);
      print_bond(b, n);
    }
  }
  else {
    if(argc - optind < 2) {
      printf("Invalid arguments.\n");
      return 0;
    }
    n = atoi(argv[optind++]);
    p = atof(argv[optind]);
    if(n < 2 || p < 0) {
      printf("Invalid arguments.\n");
      return 0;
    }
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
  printf("\nDFS 2-Thread\n%s\n\nN: %d\n", site ? "Site" : "Bond", n);
  if(p != -1.0) printf("P: %.2f\n", p);
  printf("\n");

  omp_set_num_threads(2);
  #pragma omp parallel
  {
    percolate(a, b, n, omp_get_thread_num());
  }
  join_clusters(a, b, n);
  printf("Time: %.4f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
  return 0;
}