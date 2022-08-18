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
Bond* file_bond(char*, int*);
void free_bond(Bond*);
void print_bond(Bond*, int);

#endif