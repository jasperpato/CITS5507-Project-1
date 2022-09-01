#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>

#include "./constant.h"

typedef struct Cluster {
  short *rows, *cols;
  int id, size, height, width; // id is initial site s index: s.r*n+s.c
  int* sites;
} Cluster;

typedef struct CPArray {
  Cluster** cls;
  int size;
} CPArray;

Cluster* cluster(int, int);
CPArray* cluster_array();
void scan_clusters(CPArray*, short*, int*);

#endif