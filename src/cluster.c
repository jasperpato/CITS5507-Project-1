#include "../include/cluster.h"

Cluster* cluster(int r, int c) {
  Cluster *cl = (Cluster*)calloc(1, sizeof(Cluster));
  if(!cl) return NULL;
  cl->rows = (short*)calloc(N, sizeof(short));
  cl->cols = (short*)calloc(N, sizeof(short));
  if(!cl->rows || !cl->cols) return NULL;
  cl->id = r*N+c;
  cl->rows[r] = 1;
  cl->cols[c] = 1;
  cl->height = 1;
  cl->width = 1;
  cl->size = 1;
  cl->sites = calloc(EST_CLUSTER_SIZE, sizeof(int));
  if(!cl->sites) return NULL;
  cl->sites[0] = r*N+c;
  return cl;
}
