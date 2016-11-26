// Wojciech Oziebly 25.11.2016
// Pracownia 8 Zadanie 1
// Wzorzec projektowy "I'll do it for you"

#include "sem.h"

#include <pthread.h>
#include <assert.h>

void setup_mutex(pthread_mutex_t* m)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,
            PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(m, &attr);
}

void checked_unlock(pthread_mutex_t* m)
{
    int e = pthread_mutex_unlock(m);
    assert(e == 0);
}

void sem_init(sem_t* sem, unsigned value)
{
    sem->size = value;
    sem->active = 0;
    sem->waiting = 0;
    setup_mutex(&(sem->counter_mutex));
    setup_mutex(&(sem->cs_mutex));
    pthread_cond_init(&(sem->blocked), NULL);
}

void sem_wait(sem_t* sem)
{
    pthread_mutex_lock(&(sem->counter_mutex));
    if(sem->active == sem->size)
    {
        sem->waiting++;
        checked_unlock(&(sem->counter_mutex));
        pthread_mutex_lock(&(sem->cs_mutex));
        pthread_cond_wait(&(sem->blocked), 
                            &(sem->cs_mutex));   
        checked_unlock(&(sem->cs_mutex));
    } 
    else 
    {
        sem->active++;
        checked_unlock(&(sem->counter_mutex));
    }
}

void sem_post(sem_t* sem)
{
    pthread_mutex_lock(&(sem->counter_mutex));

    if(sem->waiting > 0)
    {
        sem->waiting--;
        pthread_cond_signal(&(sem->blocked));
    }
    else
        sem->active--;
    checked_unlock(&(sem->counter_mutex));
}

void sem_getvalue(sem_t* sem, int* val)
{ 
    pthread_mutex_lock(&(sem->counter_mutex));
    *val = sem->active;
    checked_unlock(&(sem->counter_mutex));
}

