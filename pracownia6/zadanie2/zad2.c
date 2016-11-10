#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <ctype.h>

#define STACK_SIZE 16384

char buffer[512];
int words;
int characters;
int STOP = 0;

ucontext_t uctx_main, uctx_get, uctx_delete, uctx_print;

void getAndCountWords(void)
{
    while(1)
    {
        if(fgets(buffer, 512, stdin) == NULL)
        {
            STOP = 1;
            printf("Number of words = %i\n", words);
        }
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
        // printf("Swaping get to delete\n");
        swapcontext(&uctx_get, &uctx_delete);
    }
}

void deleteBlank(void)
{
    while(STOP == 0)
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
        buffer[i - offset] = buffer[i]; // move '\0'
        // printf("Swaping delete to print\n");
        swapcontext(&uctx_delete, &uctx_print);
    }
}

void countCharacters(void)
{
    while(STOP == 0)
    {
        int i;
        for( i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++);
        characters += i;
        printf("%s\n", buffer);
        // printf("Swaping print to get\n");
        swapcontext(&uctx_print, &uctx_get);
    }
    printf("Number of characters = %i\n", characters);
}

int main(void)
{
    char get_stack[STACK_SIZE]; 
    char delete_stack[STACK_SIZE]; 
    char print_stack[STACK_SIZE];

    getcontext(&uctx_get);
    uctx_get.uc_stack.ss_sp = get_stack;
    uctx_get.uc_stack.ss_size = sizeof(get_stack);
    makecontext(&uctx_get, getAndCountWords, 0);

    getcontext(&uctx_delete);
    uctx_delete.uc_stack.ss_sp = delete_stack;
    uctx_delete.uc_stack.ss_size = sizeof(delete_stack);
    uctx_delete.uc_link = &uctx_print;
    makecontext(&uctx_delete, deleteBlank, 0);
    
    getcontext(&uctx_print);
    uctx_print.uc_stack.ss_sp = print_stack;
    uctx_print.uc_stack.ss_size = sizeof(print_stack);
    uctx_print.uc_link = &uctx_main;
    makecontext(&uctx_print, countCharacters, 0);

    swapcontext(&uctx_main, &uctx_get);

    return 0;
}
