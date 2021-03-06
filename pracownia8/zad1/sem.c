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

void checked_lock(pthread_mutex_t* m)
{
    int e = pthread_mutex_lock(m);
    assert(e == 0);
}

void checked_wait(pthread_cond_t* c, pthread_mutex_t* m)
{
    int e = pthread_cond_wait(c, m);
    assert(e == 0);
}

void sem_init(sem_t* sem, unsigned value)
{
    sem->counter = value;
    setup_mutex(&(sem->mutex));
    pthread_cond_init(&(sem->blocked), NULL);
}

void sem_wait(sem_t* sem)
{
    checked_lock(&(sem->mutex));
    sem->counter--;
    if(sem->counter < 0)
            checked_wait(&(sem->blocked), &(sem->mutex));    
    checked_unlock(&(sem->mutex));
}

void sem_post(sem_t* sem)
{
    checked_lock(&(sem->mutex));
    if(sem->counter < 0)
        pthread_cond_signal(&(sem->blocked));
    sem->counter++;
    checked_unlock(&(sem->mutex));
}

void sem_getvalue(sem_t* sem, int* val)
{ 
    checked_lock(&(sem->mutex));
    *val = sem->counter;
    checked_unlock(&(sem->mutex));
}

