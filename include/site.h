#ifndef SITE_H
#define SITE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./cluster.h"
#include "./constant.h"

typedef struct Site {
  int r, c;
  short occupied, seen;
  Cluster *cluster;
} Site;

Site* site_array(float);
Site* file_site_array(char*);
void print_site_array(Site*);

#endif