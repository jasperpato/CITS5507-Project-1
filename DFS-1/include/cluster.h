#ifndef CLUSTER_H  
#define CLUSTER_H

#include <stdlib.h>

typedef struct {
  short *rows, *cols;
  int size, height, width;
} Cluster;

Cluster* cluster(int, int, int);
void free_cluster(Cluster*);

#endif