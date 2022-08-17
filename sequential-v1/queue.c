#include "queue.h"

/*
 * Circular queue implemented as an array of sites
 * Assumes queue will not overflow 
 */

Queue* queue(int n) {
  Site** queue = malloc(n*sizeof(Site*));
  Queue* q = calloc(1, sizeof(Queue));
  q->queue=queue;
  q->size=n;
  q->front=0;
  q->back=0;
  q->full=0;
  return q;
}

void free_queue(Queue* q) {
  free(q->queue);
  free(q);
}

short is_empty(Queue* q)
{
  if(q->front == q->back && !q->full) return 1;
  return 0;
}

void enqueue(Queue* q, Site* s)
{
  q->queue[q->back] = s;
  q->back = (q->back+1)%q->size;
  if(q->front == q->back) q->full = 1;
}

Site* dequeue(Queue* q)
{
  Site* s = q->queue[q->front];
  q->front = (q->front+1)%q->size;
  q->full = 0;
  return s;
}
