#include "strdrop.h"

int strdrop(char *str, const char *set)
{
    int offset = 0;
    char *begin = str;
    while(*str != '\0')
    {
        int in = 0;
        const char* p = set;
        while(*p != '\0')
        {
            if(*p == *str)
            {
                in = 1;
                break;
            }
            p++;
        }
        if(in) offset++;
        else *(str - offset) = *str;
        str++;
    }
    *(str - offset) = *str;
    return (str - offset) - begin;
}
