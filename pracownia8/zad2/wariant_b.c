#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

char* sem_names[5] = {"/sem0", "/sem1", "/sem2", "/sem3", "/sem4"};
sem_t* forks[5];
pid_t children[5];

void eat()
{
    usleep((rand()%10) * 100);
}

void think()
{
    usleep((rand()%11) * 99);
}

void take_forks(int i)
{
    sem_wait(forks[i]);
    sem_wait(forks[(i+1)%5]);
}

void put_forks(int i)
{
    sem_post(forks[i]);
    sem_post(forks[(i+1)%5]);
}

void dine(int i)
{
    while(1)
    {
        think();
        take_forks(i);
        eat();
        put_forks(i);
    }
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
    int i;
    for(i = 0; i < 5; i++)
        forks[i] = sem_open(sem_names[i], O_CREAT, 0600, 1);
}

void sigint_handler(int signum)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        if(kill(children[i], SIGTERM) == 0)
            printf("Killed %d philosopher\n", i);
    }
    for(i = 0; i < 5; i++)
        sem_unlink(sem_names[i]);

}

int main()
{
    open_semaphores();
    int i;
    for(i = 0; i < 5; i++)
        create_philosopher(i);
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
    for(int i = 0; i < 5; i++)
        wait(NULL);
}
