// Wojciech Oziebly
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

int* caldron;
sem_t* caldron_sem;
sem_t* cook_sem;
sem_t* finished_sem;


void init_caldron(int m)
{
    caldron = (int*) mmap(NULL,
                          sizeof(int),
                          PROT_WRITE | PROT_READ,
                          MAP_SHARED | MAP_ANONYMOUS,
                          -1, 0); // not a file
    *caldron = m;
}

void destroy_caldron()
{
    munmap(caldron, sizeof(int));
}

void open_semaphores()
{
    caldron_sem = sem_open("/caldron_sem", O_CREAT, S_IRWXU, 1);
    cook_sem = sem_open("/cook_sem", O_CREAT, S_IRWXU, 0);
    finished_sem = sem_open("/finished_sem", O_CREAT, S_IRWXU, 0);
}

void close_semaphores()
{
    int e;
    if((e = sem_unlink("/caldron_sem")) != 0) 
        perror("Failed to close semahore /caldron_sem ");
    if((e = sem_unlink("/cook_sem")) != 0)
        perror("Failed to close semaphore /cook_sem ");    
    if((e = sem_unlink("/finished_sem")) != 0)
        perror("Failed to close semaphore /finished_sem ");    
}

pid_t create_cook(int missionaries_count, int missionary_meatness)
{
    pid_t pid;
    if((pid = fork()) == 0)
    {
        while(missionaries_count --> 0)
        {
            sem_wait(cook_sem);
            *caldron = missionary_meatness;
            sem_post(finished_sem);
        }
        sem_wait(cook_sem);
        exit(0);
    }
    return pid;
}

pid_t create_tribeman(int id)
{
    pid_t pid;
    if((pid = fork()) == 0)
    {
        while(1)
        {
            sem_wait(caldron_sem);
            if(*caldron > 0)
            {
                (*caldron)--;
            }
            else
            {
                sem_post(cook_sem);
                printf("Tribesman %d woke up cook!\n", id);
                sem_wait(finished_sem);
                (*caldron)--;

            }
            printf("Tribesman %d just ate!\n", id);
            sem_post(caldron_sem);
            usleep((rand() % 10) * 100000);
        }
    }
    return pid;
}

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        printf("Invalid number of arguments!\n"
                "Should be 3. 1-tribesman_number 2-missionary_meatness 3-missionary_count");
        return 0;
    }
    int tribesman_number = atoi(argv[1]);    
    int missionary_meatness = atoi(argv[2]);
    int missionaries_count = atoi(argv[3]);

    init_caldron(missionary_meatness);
    open_semaphores();
    
    pid_t cook_pid = create_cook(missionaries_count, missionary_meatness);
    
    pid_t* tribesman_pids = calloc(tribesman_number, sizeof(pid_t));
    int i;
    for(i = 0; i < tribesman_number; i++)
        tribesman_pids[i] = create_tribeman(i);

    waitpid(cook_pid, NULL, 0);
    for(i = 0; i < tribesman_number; i++)
        kill(tribesman_pids[i], SIGTERM);

    close_semaphores();
    destroy_caldron();
}
