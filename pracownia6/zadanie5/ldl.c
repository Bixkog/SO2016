#include "strdrop.h"
#include "strcnt.h"
#include <stdio.h>

int main()
{
    char s[]= "Ala ma kota, a kot ma Ale";
    char* set = "aoeyiu";
    int count = strcnt(s, set);
    int left = strdrop(s, set);
    printf("Characters is string from set: %d\n", count);
    printf("Characters left: %d\n",left);
    printf("%s\n", s);
}
