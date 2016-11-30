#ifndef MY_BARRIER_H
#define MY_BARRIER_H

#include <semaphore.h>

typedef struct
{
    size_t size;
    size_t waiting;
    sem_t counter_sem;
    sem_t blocker_sem;

} barrier_t;

barrier_t* barrier_init(size_t size);
void barrier_wait(barrier_t* barrier);
void barrier_destroy(barrier_t* barrier);

#endif
