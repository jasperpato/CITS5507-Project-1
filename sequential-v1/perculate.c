#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "site.h"
#include "queue.h"

void perculate(Site* a, int n)
{
  Queue* q = queue(n*n); // circular queue
  int max_size = 1;

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
      }
      while(!is_empty(q)) {
        Site* s = dequeue(q); // Next site in the current cluster

        // indices of neighbours
        int is[] = {
          s->r*n+(s->c+n-1)%n, // left
          s->r*n+(s->c+n+1)%n, // right
          ((s->r+n-1)%n)*n+s->c, // top
          ((s->r+n+1)%n)*n+s->c // bottom
        };
        // update stats based on neighbours
        for(int i = 0; i < 4; ++i) {
          Site* nb = &a[is[i]]; // Neighbouring site
          if(!nb->marked && nb->occupied) {

            // update s and n
            nb->marked = 1;
            
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
  // find whether lattice perculates (this could be parallelised)
  short perc = 0;
  for(int r = 0; r < n; ++r) {
    for(int c = 0; c < n; ++c) {
        Site* s = &a[r*n+c];
        if(s->occupied) { // otherwise rows and cols memory has not be allocated
          int rs = 0, cs = 0;
          for(int i = 0; i < n; ++i) rs += s->rows[i];
          for(int i = 0; i < n; ++i) cs += s->cols[i];
          if(rs == n || cs == n) perc = 1;
        }
    }
  }
  free_site_array(a, n);

  printf("Perculates: %s\n", perc ? "True" : "False");
  printf("Max cluster size: %d\n", max_size);
}

int main(int argc, char *argv[])
{
  Site* a;
  int n;
  if(strcmp(argv[1], "-f") == 0) {
    a = file_site_array(argv[2], &n);
    printf("N: %d\n\n", n);
  }
  else {
    n = atoi(argv[1]);
    float p = atof(argv[2]);
    printf("N: %d\nP: %.2f\n\n", n, p);
    srand(time(NULL));
    a = site_array(n, p);
  }
  print_site_array(a, n);
  perculate(a, n);

  return 0;
}