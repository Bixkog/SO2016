// Wojciech Oziebly 281539
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "sem.h"

sem_t tabacco; 
sem_t paper; 
sem_t matches; 
sem_t smoke;
pthread_t guys[4];
int turn;

void check(int k)
{
    if(k != turn) printf(" WRONG!!!!!\n");
    else printf("\n");
}

void* smoker_with_tabacco(void* arg)
{
    while(1)
    {
        sem_wait(&matches);
        int x; sem_getvalue(&paper, &x);
        if(x == 1)
            sem_wait(&paper);
        else
        {
            sem_post(&matches);
            continue;
        }
        printf("Tabacco smoker smokes cigarette."); check(2);
        usleep(10000);
        sem_post(&smoke);
    }
}

void* smoker_with_paper(void* arg)
{
    while(1)
    {
        sem_wait(&matches);
        int x; sem_getvalue(&tabacco, &x);
        if(x == 1)
            sem_wait(&tabacco);
        else
        {
            sem_post(&matches);
            continue;
        }
        printf("Paper smoker smokes cigarette."); check(1);
        usleep(10000);
        sem_post(&smoke);
    }
}

void* smoker_with_matches(void* arg)
{
    while(1)
    {
        sem_wait(&tabacco);
        int x; sem_getvalue(&paper, &x);
        if(x == 1)
            sem_wait(&paper);
        else
        {
            sem_post(&tabacco);
            continue;
        }
        printf("Matches smoker smokes cigarette."); check(0);
        usleep(10000);
        sem_post(&smoke);
    }
}

void* dealer(void* arg)
{
    while(1)
    {
        sem_wait(&smoke);
        turn = rand()%3;
        switch(turn)
        {
        case 0:
            sem_post(&tabacco);
            sem_post(&paper);
            break;
        case 1:
            sem_post(&tabacco);
            sem_post(&matches);
            break;
        case 2:
            sem_post(&paper);
            sem_post(&matches);
            break;
        }
    }
}

void init_semaphores()
{
    sem_init(&tabacco, 0);
    sem_init(&paper, 0);
    sem_init(&matches, 0);
    sem_init(&smoke, 1);
}

void create_situation()
{
    pthread_create(&guys[0], NULL, dealer, NULL);
    pthread_create(&guys[1], NULL, smoker_with_tabacco, NULL);
    pthread_create(&guys[2], NULL, smoker_with_paper,  NULL);
    pthread_create(&guys[3], NULL, smoker_with_matches, NULL);
}

int main()
{
    init_semaphores();
    create_situation();
    int i;
    for(i = 0; i < 4; i++)
        pthread_join(guys[i], NULL);
}
