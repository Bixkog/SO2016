// Wojciech Oziebly
// Pierwszy parametr - liczba watkow
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "list.h"

int n = 1000;
list l;
pthread_mutex_t counters_m = 
    PTHREAD_MUTEX_INITIALIZER;
int reads, writes, deletitions;

void* reader(void* arg)
{
    while(1)
    {
        //usleep(rand()%500);
        printf("Reading!\n");
        pthread_mutex_lock(&(counters_m));
        reads++;
        pthread_mutex_unlock(&(counters_m));
        search(&l, rand()%n);
    }
}

void* writer(void* arg)
{
    while(1)
    {
        //usleep(rand()%1000);
        printf("Write!\n");
        pthread_mutex_lock(&(counters_m));
        writes++;
        pthread_mutex_unlock(&(counters_m));
        add(&l, rand()%n);
    }
}

void* deleter(void* arg)
{
    while(1)
    {
        //usleep(rand()%1000);
        printf("Delete!\n");
        pthread_mutex_lock(&(counters_m));
        deletitions++;
        pthread_mutex_unlock(&(counters_m));
        delete(&l, rand()%n);
    }
}

pthread_t* create_workers()
{
    int writers_count = (rand()%13) + 1;
    int deleters_count = (rand()%7) + 1;
    int readers_count = n - writers_count - deleters_count;
    pthread_t* workers = calloc(n, sizeof(pthread_t));
    int i = 0;
    for(; i < readers_count; i++)
        pthread_create(&(workers[i]), NULL, reader, NULL);
    for(; i < readers_count + writers_count; i++)
        pthread_create(&(workers[i]), NULL, writer, NULL);
    for(; i < n; i++)
        pthread_create(&(workers[i]), NULL, deleter, NULL);
    return workers;
}

void fill_list(list* l)
{
    list_init(l);
    int i;
    for(i = 0; i < n; i++)
        add(l, rand()%n);
}

int main(int argc, char** argv)
{
    if(argc == 2) n = atoi(argv[1]);
    fill_list(&l); 
    pthread_t* workers = create_workers();
    sleep(10);
    int i;
    for(i = 0; i < n; i++)
        pthread_cancel(workers[i]);
    list_destroy(&l);
    free(workers);
    printf("Reads: %d\n", reads);
    printf("Writes: %d\n", writes);
    printf("Deletitions: %d\n", deletitions);
}


