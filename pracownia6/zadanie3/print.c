#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char * const argv)
{
    int delete_output = atoi(argv[i]);
    char buffer[512];
    while(read(delete_output, buffer, 512) != 0)
    {
        int i;
        for( i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++);
        characters += i;
        printf("%s\n", buffer);
    }
    close(delete_output);
    printf("Number of characters = %i\n", characters);
}
