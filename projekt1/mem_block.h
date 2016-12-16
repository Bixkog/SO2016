#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include <sys/queue.h>
#include <types.h>

// count 16? bytes of mb_data to allocated memory 
// maybe reduce mb_size?
// lock ma_lock before entering arena?

typedef struct mem_block 
{
    LIST_ENTRY(mem_block) mb_list;
    // mb_size > 0 => block free
    // mb_size < 0 => block allocated
    ssize_T mb_size;
    union {
        // valid if block is free
        LIST_ENTRY(mem_block) mb_free_list;
        // valid if block is allocated
        uint64_t mb_data[0]; //
    }
} mem_block_t;

#endif
