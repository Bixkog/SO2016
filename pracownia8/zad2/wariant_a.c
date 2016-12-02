// Wojciech Oziebly
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "sem.h"

size_t n = 5;
pthread_t* philosophers;
sem_t* forks;
int* numbers;

void think(int i)
{
    printf("Philosopher %d thinking.\n", i);
    usleep((pthread_self() % 7) * 10);
}

void eat(int i)
{
    printf("Philosopher %d eating.\n", i);
    usleep((pthread_self() % 11) * 10);
}

void take_forks(size_t i)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if(i < (i+1)%n)
    {
        sem_wait(&(forks[i]));
        sem_wait(&(forks[(i+1)%n]));
    }
    else
    {
        sem_wait(&(forks[(i+1)%n]));
        sem_wait(&(forks[i]));
    }
}

void put_forks(int i)
{
    sem_post(&(forks[i]));
    sem_post(&(forks[(i+1)%n]));
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

void* dine(void* i)
{
    int id = *((int*)i);
    while(1)
    {
        think(id);
        take_forks(id);
        eat(id);
        put_forks(id);
    }
}

void init_forks()
{
    forks = calloc(n, sizeof(sem_t));
    size_t i;
    for(i = 0; i < n; i++)
        sem_init(&(forks[i]), 1);
}

void init_numbers()
{
    numbers = calloc(n, sizeof(int));
    size_t i;
    for(i = 0; i < n; i++)
        numbers[i] = i;
}

void create_philosophers()
{
    init_forks();
    init_numbers();
    philosophers = calloc(n, sizeof(pthread_t));
    size_t i;
    for(i = 0; i < n; i++)
        pthread_create(&(philosophers[i]), NULL, dine,(void*)&(numbers[i]));
}

void free_all()
{
    free(forks);
    free(numbers);
    free(philosophers);
}

void sigint_handler(int signum)
{
    size_t i;
    for(i = 0; i < n; i++)
    {
        int e = pthread_cancel(philosophers[i]);
        if(!e) printf("Canceling philosopher %lu\n", i);
        else printf("Unable to cancel philosopher %lu\n", i);
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

int main(int argc, char** argv)
{
    if(argc == 2) n = atoi(argv[1]);
    block_sigint();
    create_philosophers();
    
    struct sigaction act;
    act.sa_handler = sigint_handler;
    set_sigint_handler(&act);
    
    void* res;
    size_t i;
    for(i = 0; i < n; i++)
    {
        pthread_join(philosophers[i], &res);
        if(res == PTHREAD_CANCELED) printf("Canceled philosopher %lu\n", i);
    }
    free_all();
    return 0;
}

