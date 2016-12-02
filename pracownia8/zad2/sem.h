// Wojciech Oziebly 25.11.2016
// Pracownia 8 Zadanie 1
#ifndef MY_SEM_T_HPP
#define MY_SEM_T_HPP

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t blocked;
    int counter;
} sem_t;

void sem_init(sem_t* sem, unsigned value);
void sem_wait(sem_t* sem);
void sem_post(sem_t* sem);
void sem_getvalue(sem_t* sem, int* val);

#endif

