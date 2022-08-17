#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "site.h"
#include "queue.h"

void perculate(Site* a, int n)
{
  Queue* q = queue(n*n); // circular queue
  int max_size = 0;

  // printf("Initialised queue.\n\n");

  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
      Site* p = &a[r*n+c]; // Site that begins a new cluster
      if(p->occupied) {
        p->marked = 1;

        // allocate memory for rows, cols of this new cluster
        p->rows = calloc(n, sizeof(short));
        p->cols = calloc(n, sizeof(short));
        p->rows[r] = 1;
        p->cols[c] = 1;

        enqueue(q, p);
        // printf("Enqueued new cluster site (%d, %d).\n", r, c);
      }
      while(!is_empty(q)) {
        Site* s = dequeue(q); // Next site in the current cluster
        // printf("Dequeued cluster site (%d, %d).\n", s->r, s->c);

        // indices of neighbours
        int is[] = { s->r*n+(s->c-1)%n, s->r*n+(s->c+1)%n, ((s->r-1)%n)*n+c, ((s->r+1)%n)*n+c };

        // update stats based on neighbours
        for(int i = 0; i < 4; ++i) {
          Site* n = &a[is[i]]; // Neighbouring site
          if(!n->marked && n->occupied) {
            
            // printf("Occupied neighbour (%d, %d)\n", n->r, n->c);
            
            // update s
            s->size += 1;
            if(s->size > max_size) max_size = s->size;
            s->rows[n->r] = 1;
            s->cols[n->c] = 1;

            // update n
            n->marked = 1;
            n->size = s->size;
            n->rows = s->rows;
            n->cols = s->cols;

            enqueue(q, n);
          }
        }
      }
    }   
  }
  free_queue(q);
  // printf("Finished perculating. Freed queue.\n");

  // find whether lattice perculates (this could be parallelised)
  short perc = 0;
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
        Site* s = &a[r*n+c];
          if(s->occupied) { // otherwise rows and cols memory will not be allocated
          int rs = 0, cs = 0;
          // printf("Checking collected rows for site (%d, %d).\n", r, c);
          for(int i = 0; i < n; ++i) {
            // printf("%d ", s->rows[i]);
            rs += s->rows[i];
          }
          // printf("\nChecking collected cols for site (%d, %d).\n", r, c);
          for(int i = 0; i < n; ++i) {
            // printf("%d ", s->cols[i]);
            cs += s->cols[i];
          }
          if(rs == n-1 || cs == n-1) perc = 1;
        }
    }
  }
  free_site_array(a, n);

  printf("Perculates: %s\n", perc ? "True" : "False");
  printf("Max cluster size: %d\n", max_size);
}

int main(int argc, char *argv[])
{  
  int n = atoi(argv[1]);
  // float p = atof(argv[2]);
  srand(time(NULL));

  // Site* a = site_array(n, p);
  Site* a = file_site_array("lattice.txt", n);
  print_site_array(a, n);

  perculate(a, n);

  return 0;
}