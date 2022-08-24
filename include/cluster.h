#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>

#include "./constant.h"

typedef struct {
  short *rows, *cols;
  int id, size, height, width; // id is initial site s index: s.r*n+s.c
} Cluster;

Cluster* cluster(int, int);
void copy_cluster(Cluster*, Cluster*);
void free_cluster(Cluster*);

#endif