
#include "../include/stack.h"

Stack* stack() {
  Stack* st = calloc(1, sizeof(Stack));
  if(!st) return NULL;
  st->stack = calloc(N*N, sizeof(Site*));
  if(!st->stack) return NULL;
  return st;
}

short is_empty(Stack* st) {
  return st->first == st->last;
}

void add(Stack* st, Site* s) {
  st->stack[st->last++] = s;
}

Site* pop(Stack* st) {
  return st->stack[st->first++];
}