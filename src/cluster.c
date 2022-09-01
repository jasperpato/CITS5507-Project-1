#include "../include/cluster.h"

Cluster* cluster(int n, int r, int c) {
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
  cl->sites = calloc(n*n, sizeof(int));
  if(!cl->sites) return NULL;
  cl->sites[0] = r*n+c;
  return cl;
}

CPArray* cluster_array(int n_threads, int max_clusters) {
  CPArray* cpa = calloc(n_threads, sizeof(CPArray));
  for(int i = 0; i < n_threads; ++i) cpa[i].cls = calloc(max_clusters, sizeof(Cluster*));
  return cpa;
}

void free_cluster(Cluster *cl) {
  free(cl->rows);
  free(cl->cols);
  free(cl->sites);
  free(cl);
}

void free_cparray(CPArray* cpa, int n_threads) {
  for(int i = 0; i < n_threads; ++i) free(cpa[i].cls);
  free(cpa);
}

void scan_clusters(CPArray* cpa, int n, int n_threads, short *perc, int *max) {
  short p = 0;
  int m = 0;
  // #pragma omp parallel for reduction(max: m)
  for(int i = 0; i < n_threads; ++i) {
    for(int j = 0; j < cpa[i].size; ++j) {
      Cluster *cl = cpa[i].cls[j];
      if(cl->id == -1) continue;
      if(cl->size > m) m = cl->size;
      if(p) continue;
      if(cl->width == n || cl->height == n) p = 1;
      // free_cluster(cl);
    }
  }
  *perc = p;
  *max = m;
  // free_cparray(cpa, n_threads);
}

