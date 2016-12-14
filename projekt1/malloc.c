#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mem_arena.h"
#include "mem_block.h"

void* malloc(size_t size)
{
    void* alloc_pointer;
    if(size >= PAGE_SIZE*2)
    {
        alloc_pointer = allocate_big_block(size);
        if(alloc_pointer == NULL)
            errno = ENOMEM;
        return alloc_pointer;
    }
    else
    {
        mem_arena_t* arena;
        LIST_FOREACH(arena, &arena_list, ma_list)
        {
            alloc_pointer = allocate_block(arena, size);
            if(alloc_pointer != NULL)
                return alloc_pointer;
        }
        alloc_pointer = allocate_big_block(size);
        if(alloc_pointer == NULL)
            errno = ENOMEM;
        return alloc_pointer;
    }
}

void* calloc(size_t count, size_t size)
{
    void* allocated_memory = malloc(count*size);
    if(allocated_memory == NULL)
        return NULL;
    return memset(allocated_memory, 0, count*size);
}

void free(void* address)
{
    if(adress == NULL)
        return;
    mem_arena_t* arena = find_arena((uint64_t) address);
    if(arena == NULL)
        return; // segfault?
    pthread_mutex_lock(&(arena->ma_lock));
    uint64_t offset = 24; // distance in mem_block_t from beginning to mb_data
    mem_block_t* block = (mem_block_t*)((uint64_t) address  - offset);
    if(block->mb_size > 0) // double free?
    {
        pthread_mutex_unlock(&(arena->ma_lock));
        return;
    }
    block->mb_size = -(block->mb_size);
    
    mem_block_t* next_block = LIST_NEXT(block, mb_list);
    mem_block_t* prev_block = LIST_PREV(block, mb_list);
    mem_block_t* prev_free_block = prev_block;

    if(next_block == NULL || next_block->mb_size < 0 &&
       prev_block == NULL || prev_block->mb_size < 0)
    {
        // freeing last block -> free arena?
        if(next_block == NULL && prev_block == NULL)
        {
            pthread_mutex_lock(&arena_list_mutex);

            LIST_REMOVE(arena, ma_list);
            pthread_mutex_unlock(&arena_list_mutex);
            pthread_mutex_unlock(&(arena->ma_lock));
            destroy_arena(arena);
            return;
        }
        // find prev free block
        while(prev_free_block && prev_free_block->mb_size < 0)
            prev_free_block = LIST_PREV(prev_free_block, mb_list);
        // insert to free blocks
        // should be done after concatenation of free blocks, maybe able to omit
        if(prev_free_block != NULL)
            LIST_INSERT_AFTER(prev_free_block, block, mb_free_list);
        else
            LIST_INSERT_HEAD(&(arena->ma_freeblks), block, mb_free_list);
    }
    else if(next_block == NULL || next_block->mb_size < 0 &&
            prev_block->mb_size > 0)
    {
        LIST_REMOVE(block, mb_list);
        prev_block->mb_size += sizeof(mem_block_t) + block->mb_size;
    }
    else if(next_block->mb_size > 0 &&
            prev_block == NULL || prev_block->mb_size < 0)
    {
        // block <- next_block
        block->mb_size += sizeof(mem_block_t)  + next_block->mb_size;
        LIST_INSERT_BEFORE(block, next_block, ma_freeblks);
        // remove next_block from lists
        LIST_REMOVE(next_block, mb_list);
        LIST_REMOVE(next_block, mb_free_list);
    }
    else if(next_block->mb_size > 0 &&
            prev_block->mb_size > 0)
    {
        // prev_block <- block + next_block
        prev_block->mb_size += sizeof(mem_block_t) + block->mb_size +
                               sizeof(mem_block_t) + next_block->mb_size;
        LIST_REMOVE(block, mb_list);
        LIST_REMOVE(next_block, mb_list);

        LIST_REMOVE(next_block, mb_free_list);
    }
    pthread_mutex_unlock(&(arena->ma_lock));
    return;
}
