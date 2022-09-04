#ifndef BOND_H
#define BOND_H

#include <stdlib.h>
#include <stdio.h>

#include "./util.h"
#include "./site.h"

typedef struct Bond {
  short *v, *h;
} Bond;

Bond* bond(int, float);
Bond* file_bond(char*, int);
void print_bond(Bond*, int);
void free_bond(Bond*);

#endif