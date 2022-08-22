#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>

typedef struct {
  short *rows, *cols;
  int id, size, height, width; // id is initial site s index: s.r*n+s.c
} Cluster;

Cluster* cluster(int, int, int);
void free_cluster(Cluster*);

#endif