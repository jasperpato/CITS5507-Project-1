#ifndef SITE_H
#define SITE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./cluster.h"
#include "./constant.h"

typedef struct {
  int r, c;
  short occupied, seen;
  Cluster *cluster; // MAYBE: pointer to a cluster pointer (necessary when a collection of sites need to switch cluster)
} Site;

Site* site_array(float);
Site* file_site_array(char*);
void free_site_array(Site*);
void print_site_array(Site*);

#endif