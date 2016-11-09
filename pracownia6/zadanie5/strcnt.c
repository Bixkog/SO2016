#include "strcnt.h"

int strcnt(const char* str, const char* set)
{
    int counter = 0;
    int isCounted[256] = {};
    while(*str != '\0')
    {
        const char* p = set;
        while(*p != '\0')
        {
            if(*p == *str)
            {
                if(!isCounted[(int)*str])
                {
                    isCounted[(int)*str] = 1;
                    counter++;
                }
                break;
            }
            p++;
        }
        str++;
    }
    return counter;
}
