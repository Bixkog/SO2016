#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *const argv[])
{
    char buffer[512];
    int pipe_write_fd = atoi(argv[1]);
    while(fgets(buffer, 512, stdin) != NULL)
    {
        char* p = buffer;
        int at_word = (*p != ' ');
        while(*p != '\0' && *p!= '\n')
        {
            if(*p == ' ' && at_word)
            {
                words++;
                at_word = 0;
            }
            else if(!at_word)
            {
                at_word = 1;
            }
            p++;
        }
        write(pipe_write_fd, buffer, strlen(buffer));
    }
    printf("Number of words = %i\n", words);
    close(pipe_write_fd);
    return 0;
}



