#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "site.h"
#include "bond.h"
#include "queue.h"

short has_neighbours(Site* a, int n, Site* s, Bond* b) {
  // indices of bonds
  int ib[] = {
    s->r*n+s->c, // left
    s->r*n+(s->c+n+1)%n, // right
    s->r*n+s->c, // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  for(int i = 0; i < 4; ++i) {
    if((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]])) return 1;
  }
  return 0;
}

/**
 * @return Site pointer array of length 4 containing addresses of unmarked, connected neighbours
 *         Ordered left, right, top, bottom
 *         Each position contains site pointer if unmarked, connected neighbour else NULL
 */
Site** get_neighbours(Site* a, int n, Site* s, Bond* b) {
  // indices of neighbours
  int is[] = {
    s->r*n+(s->c+n-1)%n, // left
    s->r*n+(s->c+n+1)%n, // right
    ((s->r+n-1)%n)*n+s->c, // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  // indices of bonds
  int ib[] = {
    s->r*n+s->c, // left
    s->r*n+(s->c+n+1)%n, // right
    s->r*n+s->c, // top
    ((s->r+n+1)%n)*n+s->c // bottom
  };
  Site** nbs = malloc(4*sizeof(Site*));
  for(int i = 0; i < 4; ++i) {
    Site* nb = &a[is[i]];
    if(!nb->marked && ((!b && nb->occupied) || (b && ((i<2 && b->h[ib[i]]) || (i>=2 && b->v[ib[i]]))))) {
      nb->marked = 1; // must mark site here for n=2 case
      nbs[i] = nb;
    }
    else nbs[i] = NULL;
  }
  return nbs;
}

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
        // update stats based on neighbours
        for(int i = 0; i < 4; ++i) {
          if(nbs[i]) {
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
        if((!b && s->occupied) || (b && has_neighbours(a, n, s, b))) { // otherwise rows and cols memory has not be allocated
          int rs = 0, cs = 0;
          for(int i = 0; i < n; ++i) rs += s->rows[i];
          for(int i = 0; i < n; ++i) cs += s->cols[i];
          if(rs == n || cs == n) perc = 1;
        }
    }
  }
  free_site_array(a, n);
  if(b) free_bond(b);

  printf("\npercolates: %s\n", perc ? "True" : "False");
  printf("Max cluster size: %d\n", max_size);
}

int main(int argc, char *argv[])
{
  Site* a;
  int n;
  short b = 0;
  if(strcmp(argv[1], "-b") == 0) b = 1; // site or bond
  if(strcmp(argv[2], "-f") == 0) {
    if(!b) {
      a = file_site_array(argv[3], &n);
      printf("N: %d\n\n", n);
    }
    else {
      // file_bond_array
    }
  }
  else {
    n = atoi(argv[2]);
    float p = atof(argv[3]);
    printf("N: %d\nP: %.2f\n\n", n, p);
    srand(time(NULL));
    if(!b) {
      a = site_array(n, p, 1);
      print_site_array(a, n);
      percolate(a, n, NULL);
    }
    else {
      a = site_array(n, p, 0);
      Bond* b = bond(n, p);
      print_bond(b, n);
      percolate(a, n, b);
    }
  }
  return 0;
}