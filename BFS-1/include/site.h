#ifndef SITE_H
#define SITE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

typedef struct {
  int r, c, *size;
  short occupied, seen;
  short *rows, *cols; // boolean if site is connected to another site of that row/col
} Site;

Site* site_array(int, float);
Site* file_site_array(char*, int*);
void free_site_array(Site*, int);
void print_site_array(Site*, int);

#endif