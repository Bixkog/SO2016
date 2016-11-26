#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "sem.h"

pthread_t philosophers[5];
sem_t forks[5];
int numbers[5] = {0, 1, 2, 3, 4};

void think()
{
    usleep((pthread_self() % 7) * 10);
}

void eat()
{
    usleep((pthread_self() % 11) * 10);
}

void take_forks(int i)
{
    sem_wait(&(forks[i]));
    sem_wait(&(forks[(i+1)%5]));
}

void put_forks(int i)
{
    sem_post(&(forks[i]));
    sem_post(&(forks[(i+1)%5]));
}

void* dine(void* i)
{
    int id = *((int*)i);
    while(1)
    {
        think();
        take_forks(id);
        eat();
        put_forks(id);
    }
}

void init_forks()
{
    int i;
    for(i = 0; i < 5; i++)
        sem_init(&(forks[i]), 1);
}

void create_philosophers()
{
    init_forks();
    int i;
    for(i = 0; i < 5; i++)
        pthread_create(&(philosophers[i]), NULL, dine,(void*)&(numbers[i]));
}


void sigint_handler(int signum)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        pthread_cancel(philosophers[i]);
        printf("Canceling philosopher %d\n", i);
    }
}

sigset_t sigint_set()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    return set;
}

void block_sigint()
{
    sigset_t set = sigint_set();
    pthread_sigmask(SIG_BLOCK, &set, NULL);
}

void set_sigint_handler(struct sigaction* act)
{
    sigset_t set = sigint_set();
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    sigaction(SIGINT, act, NULL);
}

int main()
{
    block_sigint();
    create_philosophers();
    
    
    struct sigaction act;
    act.sa_handler = sigint_handler;
    set_sigint_handler(&act);

    int i;
    for(i = 0; i < 5; i++)
        pthread_join(philosophers[i], NULL);
}

