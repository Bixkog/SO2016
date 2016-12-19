#include "malloc.h"
#include <stdio.h>

void test_memory(int* p, size_t size)
{
    for(int i = 0; i < size/sizeof(int); i++)
        *(p+i) = 3;
}

int main()
{
    print_mem_structs();
    int* p = malloc(1024);
    test_memory(p, 1024);
    print_mem_structs();
    realloc(p, 0);
    print_mem_structs();
    p = malloc(2049);
    test_memory(p, 2049);
    print_mem_structs();
    p = realloc(p, 0);
    print_mem_structs();
    posix_memalign(&p, 512, 4096);
    test_memory(p, 4096);
    print_mem_structs();
    realloc(p, 0);
    print_mem_structs();
    malloc(0);
    print_mem_structs();
    free(NULL);
    print_mem_structs();
    int* tab = calloc(sizeof(int), 1000000);
    print_mem_structs();
    free(tab);
    print_mem_structs();
}
