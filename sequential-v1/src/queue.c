#include "queue.h"

/**
 * @return Queue* pointer to a circular queue array of size n of site pointers
 */
Queue* queue(int n)
{
  Site** queue = malloc(n*sizeof(Site*));
  Queue* q = calloc(1, sizeof(Queue));
  q->queue=queue;
  q->size=n;
  q->front=0;
  q->back=0;
  q->full=0;
  return q;
}

void free_queue(Queue* q)
{
  free(q->queue);
  free(q);
}

short is_empty(Queue* q)
{
  if(q->front == q->back && !q->full) return 1;
  return 0;
}

/**
 * @brief enqueue the site pointer s to queue q. Does not check for overflow
 */
void enqueue(Queue* q, Site* s)
{
  q->queue[q->back] = s;
  q->back = (q->back+1)%q->size;
  if(q->front == q->back) q->full = 1;
}

/**
 * @brief return pointer to site at the front of the queue q and remove pointer from queue
 */
Site* dequeue(Queue* q)
{
  Site* s = q->queue[q->front];
  q->front = (q->front+1)%q->size;
  q->full = 0;
  return s;
}
