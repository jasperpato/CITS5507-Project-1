#ifndef SITE_H
#define SITE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./cluster.h"

typedef struct {
  int r, c;
  short occupied, seen;
  Cluster *cluster;
} Site;

Site* site_array(int, float);
Site* file_site_array(char*, int*);
void free_site_array(Site*, int);
void print_site_array(Site*, int);

#endif