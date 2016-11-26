#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sem.h"


sem_t semaphore;

void* testProcedure(void* arg)
{
    sem_wait(&semaphore);
    int c; sem_getvalue(&semaphore, &c);
    printf("Thread %lu in, counter: %d\n", pthread_self(), c);
    sleep(1);
    sem_getvalue(&semaphore, &c);
    printf("Thread %lu out, counter: %d\n", pthread_self(), c);
    sem_post(&semaphore);
}

int main(int argc, char** argv)
{
    if(argc != 3) return 1;
    size_t threads_count = atoi(argv[1]);
    size_t sem_counter = atoi(argv[2]);
    sem_init(&semaphore, sem_counter);
    pthread_t* threads = (pthread_t*)calloc(threads_count, 
                                            sizeof(*threads));
    unsigned i;
    for(i = 0; i < threads_count; i++)
        pthread_create(&(threads[i]), NULL, testProcedure, NULL);

    for(i = 0; i < threads_count; i++)
        pthread_join(threads[i], NULL);
}
