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
  cl->sites = calloc(NUM_SITES, sizeof(int));
  if(!cl->sites) return NULL;
  cl->sites[0] = r*N+c;
  return cl;
}

CPArray* cluster_array() {
  CPArray* cpa = calloc(N_THREADS, sizeof(CPArray));
  for(int i = 0; i < N_THREADS; ++i) cpa[i].cls = calloc(MAX_CLUSTERS, sizeof(Cluster*));
  return cpa;
}

void scan_clusters(CPArray* cpa, short *perc, int *max) {
  short p = 0;
  int m = 0;
  // #pragma omp parallel for reduction(max: m)
  for(int i = 0; i < N_THREADS; ++i) {
    for(int j = 0; j < cpa[i].size; ++j) {
      Cluster *cl = cpa[i].cls[j];
      if(cl->size > m) m = cl->size;
      if(p) continue;
      if(cl->width == N || cl->height == N) p = 1;
    }
  }
  *perc = p;
  *max = m;
}
