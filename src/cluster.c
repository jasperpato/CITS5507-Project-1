#include "../include/cluster.h"

/** 
 * @return pointer to a new cluster starting from site (r, c)  
 */
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
  cl->sites = calloc(n*n, sizeof(int)); // only 2*n for border sites
  if(!cl->sites) return NULL;
  cl->sites[0] = r*n+c;
  return cl;
}

/** 
 * @return CPArray* 
 */
CPArray* cluster_array(int n_threads, int num_clusters) {
  CPArray* cpa = calloc(n_threads, sizeof(CPArray));
  for(int i = 0; i < n_threads; ++i) cpa[i].cls = calloc(num_clusters, sizeof(Cluster*));
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


