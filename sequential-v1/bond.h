#ifndef BOND_H
#define BOND_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "site.h"

typedef struct {
  short *v, *h;
} Bond;

Bond* bond(int, float);
void free_bond(Bond*);
void print_bond(Bond*, int);

#endif