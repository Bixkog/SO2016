// Wojciech Oziebly
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

size_t n = 5;
char** sem_names;
sem_t** forks;
pid_t* children;

void eat(int i)
{
    printf("Philosopher %d eating.\n", i);
    usleep((rand()%10) * 10000);
}

void think(int i)
{
    printf("Philosopher %d thinking.\n", i);
    usleep((rand()%11) * 9900);
}

void take_forks(size_t i)
{
    if(i < (i+1)%n)
    {
        sem_wait(forks[i]);
        sem_wait(forks[(i+1)%n]);
    }
    else
    {
        sem_wait(forks[(i+1)%n]);
        sem_wait(forks[i]);
    }
}
void put_forks(int i)
{
    sem_post(forks[i]);
    sem_post(forks[(i+1)%n]);
}

void dine(int i)
{
    while(1)
    {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

void init_tables()
{
    sem_names = calloc(n, sizeof(char*));
    char s[256];
    size_t i;
    for(i = 0; i < n; i++)
    {
        sprintf(s, "/sem%lu", i);
        sem_names[i] = malloc(strlen(s));
        strcpy(sem_names[i], s);
    }

    forks = calloc(n, sizeof(sem_t*));
    children = calloc(n, sizeof(pid_t));
}

void free_tables()
{
    size_t i;
    for(i = 0; i < n; i++)
        free(sem_names[i]);
    free(sem_names);
    free(forks);
    free(children);
}

void create_philosopher(int i)
{
    pid_t pid;
    if((pid = fork()) == 0)
    {
        dine(i);
    }
    else
    {
        children[i] = pid;
    }
}

void open_semaphores()
{
    size_t i;
    for(i = 0; i < n; i++)
        forks[i] = sem_open(sem_names[i], O_CREAT, 0600, 1);
}

void sigint_handler(int signum)
{
    size_t i;
    for(i = 0; i < n; i++)
    {
        if(kill(children[i], SIGTERM) == 0)
            printf("Killed %lu philosopher\n", i);
        else
            perror("Unable to kill philosopher: ");
    }
    for(i = 0; i < n; i++)
        sem_unlink(sem_names[i]);
}

int main(int argc, char** argv)
{
    if(argc == 2) n = atoi(argv[1]);
    init_tables();
    srand(time(NULL));
    open_semaphores();
    size_t i;
    for(i = 0; i < n; i++)
        create_philosopher(i);
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
    for(i = 0; i < n; i++)
        wait(NULL);
    free_tables();
}

