#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>

#include "./constant.h"

typedef struct Cluster {
  short *rows, *cols;
  int id, size, height, width; // id is initial site s index: s.r*n+s.c
  int* sites;
} Cluster;

Cluster* cluster(int, int);

#endif