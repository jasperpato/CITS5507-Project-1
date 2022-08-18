#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "site.h"
#include "bond.h"
#include "queue.h"

/**
 * @brief Check if a site has at least one bond to a neighbour (and can therefore be the start of a cluster)
 * @param a site array
 * @param n size of site array
 * @param s site to check
 * @param b bond struct
 * @return short boolean true iff has at least one neighbour
 */
short has_neighbours(Site* a, int n, Site* s, Bond* b)
{
  // indices of bonds
  int ib[] = {
    s->r*n+s->c,          // left
    s->r*n+(s->c+n+1)%n,  // right
    s->r*n+s->c,          // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    // check for a bond to neighbour
    if((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]])) return 1;
  }
  return 0;
}

/**
 * @brief Get an array of neighbour's site pointers
 * @param a site array
 * @param n size of site array
 * @param s site to examine
 * @param b bond struct, either a valid address if [-b] or NULL if [-s]
 * @return Site** array of size 4 of neighbour's site pointers. Pointer is either a valid address
 *         if there is a connection to that neighbour, otherwise NULL
 */
Site** get_neighbours(Site* a, int n, Site* s, Bond* b)
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
  Site** nbs = malloc(4*sizeof(Site*));
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    if(!nb->marked && ((!b && nb->occupied) || (b && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))))) {
      nb->marked = 1; // must mark site here for n=2 case when top neighbour == bottom neighbour
      nbs[i] = nb;
    } else nbs[i] = NULL;
  }
  return nbs;
}

/**
 * @brief Simulate percolation using BFS approach. Outputs max cluster size and percolation boolean.
 * @param a site array
 * @param n size of site array
 * @param b bond struct, either valid address if [-b] or NUll if [-s]
 */
void percolate(Site* a, int n, Bond* b)
{
  Queue* q = queue(n*n); // circular queue
  int max_size = 0;
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      Site* p = &a[r*n+c]; // Site that begins a new cluster
      if((!b && p->occupied) || (b && has_neighbours(a, n, p, b))) {
        p->marked = 1;
        max_size = max_size ? max_size : 1;

        // allocate memory for rows, cols of this new cluster
        p->rows = calloc(n, sizeof(short));
        p->cols = calloc(n, sizeof(short));
        p->rows[r] = 1;
        p->cols[c] = 1;

        enqueue(q, p);
      }
      while(!is_empty(q)) {
        Site* s = dequeue(q); // Next site in the current cluster
        Site** nbs = get_neighbours(a, n, s, b);
        for(int i = 0; i < 4; ++i) {
          if(nbs[i]) { // loop through connected, unmarked neighbours
            Site* nb = nbs[i];

            // update s and n
            *(s->size) += 1;
            nb->size = s->size;
            if(*(nb->size) > max_size) max_size = *(nb->size);

            nb->rows = s->rows;
            nb->cols = s->cols;
            nb->rows[nb->r] = 1;
            nb->cols[nb->c] = 1;

            enqueue(q, nb);
          }
        }
      }
    }   
  }
  free_queue(q);

  // find whether lattice percolates (this could be parallelised)
  short perc = 0;
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
        Site* s = &a[r*n+c];
        if((!b && s->occupied) || (b && has_neighbours(a, n, s, b))) {
          int rs = 0, cs = 0;
          for(int i = 0; i < n; ++i) rs += s->rows[i];
          for(int i = 0; i < n; ++i) cs += s->cols[i];
          if(rs == n || cs == n) perc = 1;
        }
        // else: otherwise rows and cols memory has not be allocated
    }
  }
  free_site_array(a, n);
  if(b) free_bond(b);

  printf("Percolates: %s\n", perc ? "True" : "False");
  printf("Max cluster size: %d\n", max_size);
}

/**
 * USAGE: ./percolate [-b | -s] [[-f FILENAME] | [LATTICE_SIZE PROBABILITY]]
 */
int main(int argc, char *argv[])
{
  Site* a = NULL;
  Bond* b = NULL;
  int n = 0;
  float p = -1.0;
  short bd = 0;

  if(argc < 4) {
    printf("Invalid arguments.\n");
    return 0;
  }
  if(strcmp(argv[1], "-b") == 0) bd = 1; // site or bond
  else if(strcmp(argv[1], "-s") != 0) {
    printf("Invalid arguments.\n");
    return 0;
  }
  if(strcmp(argv[2], "-f") == 0) {
    if(bd) {
      b = file_bond(argv[3], &n);
      if(!b) {
        printf("Error reading file.\n");
        return 0;
      }
      a = site_array(n, -1.0);
      print_bond(b, n);
    } else {
      a = file_site_array(argv[3], &n);
      if(!a) {
        printf("Error reading file.\n");
        return 0;
      }
      print_site_array(a, n);
    }
  } else {
    n = atoi(argv[2]);
    p = atof(argv[3]);
    if(n < 2 || p < 0) {
      printf("Invalid arguments.\n");
      return 0;
    }
    srand(time(NULL));
    if(bd) {
      b = bond(n, p);
      a = site_array(n, -1.0);
      print_bond(b, n);
    } else {
      a = site_array(n, p);
      print_site_array(a, n);
    }
  }
  printf("\nN: %d\n", n);
  if(p != -1.0) printf("P: %.2f\n", p);
  printf("\n");
  percolate(a, n, b);
  return 0;
}