#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *const argv[])
{
    char buffer[512] = "";
    if(argc != 2)
    {
        return -1;
    }
    int get_input = atoi(argv[1]);
    int words = 0;
    while(fgets(buffer, 512, stdin) != NULL)
    {
        char* p = buffer;
        int at_word = (*p != ' ');
        while(*p != '\0')
        {
            if((*p == ' ' || *p == '\n') && at_word)
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
        write(get_input, buffer, strlen(buffer));
        memset(buffer, 0, 512);
    }
    printf("Number of words = %i\n", words);
    close(get_input);
    return 0;
}



