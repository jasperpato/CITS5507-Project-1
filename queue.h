#ifndef QUEUE_H
#define QUEUE_H

#include "site.h"

typedef struct {
  Site** queue;
  int size, front, back;
  short full;
} Queue;

Queue* queue(int);
void free_queue(Queue*);
short is_empty(Queue*);
void enqueue(Queue*, Site*);
Site* dequeue(Queue*);
 
#endif
