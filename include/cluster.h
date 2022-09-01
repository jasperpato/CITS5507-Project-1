#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>
// #include <omp.h>

typedef struct Cluster {
  short *rows, *cols;
  int id, size, height, width; // id is initial site s index: s.r*n+s.c
  int* sites;
} Cluster;

typedef struct CPArray {
  Cluster** cls;
  int size;
} CPArray;

Cluster* cluster(int, int, int);
CPArray* cluster_array(int, int);
void free_cluster(Cluster*);
void free_cparray(CPArray*, int);
void scan_clusters(CPArray*, int, int, short*, int*);

#endif