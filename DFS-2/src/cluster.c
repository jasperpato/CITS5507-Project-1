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

void free_cluster(Cluster* cl) {
  free(cl->rows);
  free(cl->cols);
  free(cl);
}