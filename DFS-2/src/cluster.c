#include "../include/cluster.h"

Cluster* cluster(int r, int c, int n) {
  Cluster *cl = (Cluster*)calloc(1, sizeof(Cluster));
  if(!cl) return NULL;
  cl->rows = (short*)calloc(n, sizeof(short));
  cl->cols = (short*)calloc(n, sizeof(short));
  if(!cl->rows || !cl->cols) return NULL;
  cl->id = r*n+c;
  cl->rows[r] = 1;
  cl->cols[c] = 1;
  cl->height = 1;
  cl->width = 1;
  cl->size = 1;
  return cl;
}

/**
 * @brief copies contents of c1 into c2
 */
void copy_cluster(Cluster *c1, Cluster *c2) {
  c2->id = c1->id;
  c2->rows = c1->rows;
  c2->cols = c1->cols;
  c2->height = c1->height;
  c2->width = c1->width;
  c2->size = c1->size;
}

void free_cluster(Cluster* cl) {
  free(cl->rows);
  free(cl->cols);
  free(cl);
}