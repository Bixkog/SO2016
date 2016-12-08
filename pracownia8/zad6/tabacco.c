// Wojciech Oziebly 281539
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "sem.h"

sem_t tabacco; sem_t tabacco_smoker; int isTabacco;
sem_t paper; sem_t paper_smoker; int isPaper;
sem_t matches; sem_t matches_smoker; int isMatches;
sem_t mutex;
sem_t smoke;
pthread_t guys[4];
pthread_t pushers[3];
int turn;

void check(int k)
{
    if(k != turn) printf(" WRONG!!!!!\n");
    else printf("\n");
}

void* tabacco_pusher(void* arg)
{
    while(1)
    {
        sem_wait(&tabacco);
        sem_wait(&mutex);
        if(isPaper)
            sem_post(&matches_smoker);
        else if(isMatches)
            sem_post(&paper_smoker);
        else
            isTabacco = 1;
        sem_post(&mutex);
    }
}

void* paper_pusher(void* arg)
{
    while(1)
    {
        sem_wait(&paper);
        sem_wait(&mutex);
        if(isTabacco)
            sem_post(&matches_smoker);
        else if(isMatches)
            sem_post(&tabacco_smoker);
        else
            isPaper = 1;
        sem_post(&mutex);
    }
}

void* matches_pusher(void* arg)
{
    while(1)
    {
        sem_wait(&matches);
        sem_wait(&mutex);
        if(isTabacco)
            sem_post(&paper_smoker);
        else if(isPaper)
            sem_post(&tabacco_smoker);
        else
            isMatches = 1;
        sem_post(&mutex);
    }
}

void* smoker_with_tabacco(void* arg)
{
    while(1)
    {
        sem_wait(&tabacco_smoker);
        isPaper = 0; isMatches = 0;
        printf("Tabacco smoker smokes cigarette."); check(2);
        usleep(10000);
        sem_post(&smoke);
    }
}

void* smoker_with_paper(void* arg)
{
    while(1)
    {
        sem_wait(&paper_smoker);
        isTabacco = 0; isMatches = 0;
        printf("Paper smoker smokes cigarette."); check(1);
        usleep(10000);
        sem_post(&smoke);
    }
}

void* smoker_with_matches(void* arg)
{
    while(1)
    {
        sem_wait(&matches_smoker);
        isTabacco = 0; isPaper = 0;
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
    sem_init(&tabacco_smoker, 0);
    sem_init(&paper_smoker, 0);
    sem_init(&matches_smoker, 0);
    sem_init(&mutex, 1);
}

void create_situation()
{
    pthread_create(&guys[0], NULL, dealer, NULL);
    pthread_create(&guys[1], NULL, smoker_with_tabacco, NULL);
    pthread_create(&guys[2], NULL, smoker_with_paper,  NULL);
    pthread_create(&guys[3], NULL, smoker_with_matches, NULL);
    pthread_create(&pushers[0], NULL, tabacco_pusher, NULL);
    pthread_create(&pushers[1], NULL, paper_pusher, NULL);
    pthread_create(&pushers[2], NULL, matches_pusher, NULL);
}

int main()
{
    init_semaphores();
    create_situation();
    int i;
    for(i = 0; i < 4; i++)
        pthread_join(guys[i], NULL);
    for(i = 0; i < 3; i++)
        pthread_join(pushers[i], NULL);
}
