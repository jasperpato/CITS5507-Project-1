#ifndef STACK_H
#define STACK_H 

#include "./constant.h"
#include "./site.h"

typedef struct Stack {
  Site** stack;
  int first, last;
} Stack;

Stack* stack();
short is_empty(Stack*);
void add(Stack*, Site*);
Site* pop(Stack*);

#endif