#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int pipe_get_delete[2];
int pipe_delete_print[2];
char pipe_get_delete_s[2][20];
char pipe_delete_print_s[2][20];

char* cenv[];
char* get_argv[];
char* delete_argv[];
char* print_argv[];

void setPipes(void)
{
    pipe(pipe_get_delete);
    pipe(pipe_delete_print);
        
    sprintf(pipe_get_delete_s[0], "%d", pie_get_delete[0]);
    sprintf(pipe_get_delete_s[1], "%d", pie_get_delete[1]);
    sprintf(pipe_delete_print_s[0], "%d", pie_delete_print[0]);
    sprintf(pipe_delete_print_s[1], "%d", pie_delete_print[1]);
}

void setArgv(void)
{
    char* cenv[] = { NULL };
    char* get_argv[] = { "get", pipe_get_delete_s[0] };
    char* delete_argv[] = { "delete", pipe_get_delete_s[1], pipe_delete_print_s[0] };
    char* print_argv[] = { "print", pipe_delete_print_s[1] };
}

int main(int argc, char *argv[])
{
    setPipes();
    setArgv();


    get_pid = fork();
    if(get_pid == -1)
    {
        return -1;
    }
    else if(get_pid == 0)
    {
        close(pipe_delete_print[0]);
        close(pipe_delete_print[1]);
        close(pipe_get_delete[1]);
        execve("./get", get_argv, cenv);
        return -1;
    }

    delete_pid = fork();
    if(delete_pid == -1)
    {
        return -1;
    }
    else if(delete_pid == 0)
    {
        close(pipe_get_delete[0]);
        close(pipe_delete_print[1]);
        execve("./delete", delete_argv, cenv);
        return -1;
    }

    print_pid = fork();
    if(print_pid == -1)
    {
        return -1;
    }
    else if(print_pid == 0)
    {
        close(pipe_get_delete[0]);
        close(pipe_get_delete[1]);
        close(pipe_delete_print[0]);
        execve("./print", print_argv, cenv);
        return -1;
    }

    wait();
    wait();
    wait();
}
