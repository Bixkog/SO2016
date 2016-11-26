#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sem.h"


sem_t semaphore;

void* testProcedure(void* arg)
{
    sem_wait(&semaphore);
    printf("Thread %lu in\n", pthread_self());
    sleep(1);
    printf("Thread %lu out\n", pthread_self());
    sem_post(&semaphore);
}

int main(int argc, char** argv)
{
    if(argc != 2) return 1;
    size_t threads_count = atoi(argv[1]);
    pthread_t* threads = (pthread_t*)calloc(threads_count, sizeof(*threads));
    int i;
    for(i = 0; i < threads_count; i++)
        pthread_create(&(threads[i]), NULL, testProcedure, NULL);

    for(i = 0; i < threads_count; i++)
        pthread_join(threads[i], NULL);
}
