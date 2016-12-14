#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <sys/queue.h>
#include <pthread.h>
#include "mem_block.h"

typedef struct mem_arena
{
    pthread_mutex_t ma_lock;
    LIST_ENTRY(mem_arena) ma_list;
    LIST_HEAD(, mem_block) ma_freeblks;
    // arena size minus sizeof(mem_arena_t)
    size_t size;
    mem_block_t ma_first;
} mem_arena_t;

// lock one traversing arenas?
pthread_mutex_t arena_list_lock = PTHREAD_MUTEX_INITIALIZER;
LIST_HEAD(, mem_arena) arena_list;

#endif

