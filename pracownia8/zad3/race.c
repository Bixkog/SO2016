// Wojciech Oziebly
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/mman.h>

#include "barrier.h"

barrier_t* barrier;

void change_proc_name(char * name, char* current_name)
{
    int name_length = strlen(current_name);
    strncpy(current_name, name, name_length); // proces
    prctl(PR_SET_NAME, name); // watek
}

void ride()
{
    usleep(((rand()% 5) + 5) * 100);
}

void create_horse(int id, int laps, char* name)
{
    if(fork() == 0)
    {
        change_proc_name("horse", name);
        int i;
        for(i = 1; i <= laps; i++)
        {
            barrier_wait(barrier);
            ride();
            printf("Rider %d finished lap %d\n", id, i);
        }
        exit(0);
    }
}


sigset_t sigusr1_set()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    return set;
}

void block_sigusr1()
{
    sigset_t set = sigusr1_set();
    sigprocmask(SIG_BLOCK, &set, NULL);
}

void wait_for_sigusr1()
{
    sigset_t set = sigusr1_set();
    int sig;
    sigwait(&set, &sig);
}

pid_t create_gate(int n, char* name)
{
    pid_t pid;
    if((pid = fork()) == 0)
    {
        block_sigusr1();
        change_proc_name("gate", name);
        barrier = barrier_init(n);
        kill(getppid(), SIGUSR1);
        wait_for_sigusr1();
        barrier_destroy(barrier);
        exit(0);
    }
    return pid;
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    if(argc != 3)
    {
        printf("Wrong number of arguments! 2 arguments needed.\n");
        printf("1 - number of horseman, 2 - number of laps\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    barrier = barrier_init(n);
    int i;
    for(i = 0; i < n; i++)
        create_horse(i, k, argv[0]);
    for(i = 0; i < n; i++)
        wait(NULL);
    barrier_destroy(barrier);
}
