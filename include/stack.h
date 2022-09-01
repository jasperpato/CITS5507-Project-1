#ifndef STACK_H
#define STACK_H 

#include "./site.h"

typedef struct Stack {
  Site** stack;
  int first, last;
} Stack;

Stack* stack(int);
short is_empty(Stack*);
void add(Stack*, Site*);
Site* pop(Stack*);
void free_stack(Stack*);

#endif