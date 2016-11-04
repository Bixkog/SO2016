#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipe_get_delete[2];
int pipe_delete_print[2];
char pipe_get_delete_s[2][20];
char pipe_delete_print_s[2][20];

void setPipes(void)
{
    pipe(pipe_get_delete);
    pipe(pipe_delete_print);
        
    sprintf(pipe_get_delete_s[0], "%d", pipe_get_delete[0]);
    sprintf(pipe_get_delete_s[1], "%d", pipe_get_delete[1]);
    sprintf(pipe_delete_print_s[0], "%d", pipe_delete_print[0]);
    sprintf(pipe_delete_print_s[1], "%d", pipe_delete_print[1]);
}

int main(void)
{
    setPipes();
    char* const cenv[] = { NULL };
    char* const get_argv[] = { "./get", pipe_get_delete_s[1], NULL };
    char* const delete_argv[] = { "./delete", pipe_get_delete_s[0], pipe_delete_print_s[1], NULL };
    char* const print_argv[] = { "./print", pipe_delete_print_s[0], NULL };

    int get_pid = fork();
    if(get_pid == -1)
    {
        return -1;
    }
    else if(get_pid == 0)
    {
        close(pipe_delete_print[1]);
        close(pipe_delete_print[0]);
        close(pipe_get_delete[0]);
        execve("./get", get_argv, cenv);
        printf("Execve on get error\n");
        return -1;
    }

    int delete_pid = fork();
    if(delete_pid == -1)
    {
        return -1;
    }
    else if(delete_pid == 0)
    {
        close(pipe_get_delete[1]);
        close(pipe_delete_print[0]);
        execve("./delete", delete_argv, cenv);
        printf("Execve on delete error\n");
        return -1;
    }

    int print_pid = fork();
    if(print_pid == -1)
    {
        return -1;
    }
    else if(print_pid == 0)
    {
        close(pipe_get_delete[1]);
        close(pipe_get_delete[0]);
        close(pipe_delete_print[1]);
        execve("./print", print_argv, cenv);
        printf("Execve on print error\n");
        return -1;
    }
    
    close(pipe_get_delete[0]);
    close(pipe_get_delete[1]);
    close(pipe_delete_print[0]);
    close(pipe_delete_print[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
}
