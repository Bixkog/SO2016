#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char * const argv[])
{
    if(argc != 2)
        return -1;
    int delete_output = atoi(argv[1]);
    char buffer[512] = "";
    int characters = 0;
    int b;
    while((b = read(delete_output, buffer, 512)))
    {
        if(b == -1)
            return -1;
        characters += b;
        printf("%s\n", buffer);
        memset(buffer, 0, 512);
    }
    close(delete_output);
    printf("Number of characters = %i\n", characters);
}
