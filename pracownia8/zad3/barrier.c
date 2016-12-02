// Wojciech Oziebly
#include "barrier.h"
#include <stdlib.h>
#include <sys/mman.h>

barrier_t* barrier_init(size_t size)
{
    barrier_t* barrier = (barrier_t*) mmap(NULL,
                   sizeof(barrier_t),
                   PROT_WRITE | PROT_READ,
                   MAP_SHARED | MAP_ANONYMOUS,
                   -1, 0);
    barrier->size = size;
    barrier->waiting = 0;
    sem_init(&(barrier->counter_sem), 1, 1);
    sem_init(&(barrier->in_sem), 1, size);
    sem_init(&(barrier->out_sem), 1, 0);
    return barrier;
}

void barrier_wait(barrier_t* barrier)
{
    sem_wait(&(barrier->counter_sem));
    sem_wait(&(barrier->in_sem));
    barrier->waiting++;
    if(barrier->waiting == barrier->size)
    {
        unsigned i;
        for(i = 0; i < barrier->size - 1; i++)
            sem_post(&(barrier->out_sem));

        sem_post(&(barrier->counter_sem));
    }
    else
    {
        sem_post(&(barrier->counter_sem));
        sem_wait(&(barrier->out_sem));
        sem
        sem->waiting--;
    }
}

void barrier_destroy(barrier_t* barrier)
{
    sem_destroy(&(barrier->counter_sem));
    sem_destroy(&(barrier->blocker_sem));
    munmap(barrier, sizeof(barrier_t));
}
