#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int main(int argc, char * const argv[])
{
    if(argc != 3)
        return -1;
    char buffer[512] = "";
    int get_output = atoi(argv[1]);
    int print_input = atoi(argv[2]);
    while(read(get_output, buffer, 512) > 0)
    {
        int offset = 0;
        int i;
        for( i = 0; buffer[i] != '\0'; i++)
        {
            if(isalnum(buffer[i]))
                buffer[i - offset] = buffer[i];
            else
                offset++;
        }
        buffer[i - offset] = buffer[i]; 
        write(print_input, buffer, strlen(buffer));
        memset(buffer, 0, 512);
    }
    close(get_output);
    close(print_input);
    return 0;
}
