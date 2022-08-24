#ifndef BOND_H
#define BOND_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./site.h"
#include "./constant.h"

typedef struct {
  short *v, *h;
} Bond;

Bond* bond(float);
Bond* file_bond(char*);
void free_bond(Bond*);
void print_bond(Bond*);

#endif