#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mem_arena.h"
#include "mem_block.h"


int posix_memalign(void** memptr, size_t alignment, size_t size)
{
    if(size == 0)
    {
        *memptr = NULL;
        return 0;
    }
    // validate alignment
    if((alignment & (alignment - 1)) == 0 || alignment % sizeof(void*) != 0)
        return EINVAL;

    pthread_mutex_lock(&mem_lock);
    if(size + alignment >= (size_t) PAGE_SIZE*2)
    {
        *memptr = allocate_big_block(size, alignment);
        pthread_mutex_unlock(&mem_lock);
        if(*memptr == NULL)
            return ENOMEM;
        return 0;
    }
    else
    {
        mem_arena_t* arena;
        LIST_FOREACH(arena, &arena_list, ma_list)
        {
            *memptr = allocate_block(arena, size, alignment);
            if(*memptr != NULL)
            {
                pthread_mutex_unlock(&mem_lock);
                return 0;
            }
        }
        *memptr = allocate_big_block(size, alignment);
        pthread_mutex_unlock(&mem_lock);
        if(*memptr == NULL)
            return ENOMEM;
        return 0;
    }
}

void* malloc(size_t size)
{
    pthread_mutex_lock(&mem_lock);
    void* ptr;
    if(posix_memalign(&ptr, 8, size) == ENOMEM)
    {
        errno = ENOMEM;
        pthread_mutex_unlock(&mem_lock);
        return NULL;
    }
    pthread_mutex_unlock(&mem_lock);
    return ptr;
}

void* calloc(size_t nmemb, size_t size)
{
    pthread_mutex_lock(&mem_lock);
    void* allocated_memory = malloc(nmemb*size);
    pthread_mutex_unlock(&mem_lock);
    if(allocated_memory == NULL)
        return NULL;
    return memset(allocated_memory, 0, nmemb*size);
}

void free(void* ptr)
{
    if(ptr == NULL)
        return;
    pthread_mutex_lock(&mem_lock);
    mem_arena_t* arena = find_arena((uint64_t) ptr);
    if(arena == NULL)
    {
        pthread_mutex_unlock(&mem_lock);
        return; // segfault?
    }
    // get block from address
    mem_block_t* block = (mem_block_t*)((uint64_t) ptr - MB_STRUCT_RSIZE);
    if(block->mb_size > 0) // double free?
    {
        pthread_mutex_unlock(&(mem_lock));
        return;
    }
    block->mb_size = -(block->mb_size);

    mem_block_t* next_block = LIST_NEXT(block, mb_list);
    mem_block_t* prev_block = LIST_PREV(block, &(arena->ma_freeblks), mem_block, mb_list);
    
    if((next_block == NULL || next_block->mb_size < 0) &&
       (prev_block == NULL || prev_block->mb_size < 0))
    {
        mem_block_t* prev_free_block = find_prev_free_block(arena, block); 
        // insert to free blocks
        if(prev_free_block)
            LIST_INSERT_AFTER(prev_free_block, block, mb_free_list);
        else
            LIST_INSERT_HEAD(&(arena->ma_freeblks), block, mb_free_list);
    }
    else
    {
        if(next_block && next_block->mb_size > 0)
        {
            concat_free_blocks(block, next_block);
            next_block = LIST_NEXT(block, mb_list);
        }
        if(prev_block && prev_block->mb_size > 0)
        {
            concat_free_blocks(prev_block, block);
            block = prev_block;
            prev_block = LIST_PREV(block, &(arena->ma_freeblks), mem_block, mb_list);
        }
    }
    // unmap arena if needed
    if(prev_block == NULL && next_block == NULL && // last block
        (LIST_FIRST(&arena_list) != arena  || LIST_NEXT(arena, ma_list) == NULL)) // not last arena
    {
        LIST_REMOVE(arena, ma_list);
        destroy_arena(arena);
    }
    pthread_mutex_unlock(&mem_lock);
    return;
}

void* realloc(void* ptr, size_t size)
{
    if(ptr == NULL)
        return malloc(size);
    if(size == 0)
    {
        free(ptr);
        return ptr;
    }
    pthread_mutex_lock(&mem_lock);
    mem_arena_t* arena = find_arena((uint64_t)ptr);
    if(arena == NULL)
    {
        pthread_mutex_unlock(&mem_lock);
        return NULL;
    }
    // get block from address
    mem_block_t* block = (mem_block_t*)((uint64_t) ptr  - MB_STRUCT_RSIZE);
    size_t old_size = -(block->mb_size);
    if(old_size > size)
    {
        reduce_block(arena, block, size);
    }
    else if(old_size < size)
    {
        mem_block_t* next_block = LIST_NEXT(block, mb_list);
        if(next_block == NULL || next_block->mb_size < 0 || // no more space
           size > old_size + next_block->mb_size + MB_STRUCT_RSIZE) // not enough space
        {
            // need to move
            void* new_ptr = malloc(size);
            memcpy(new_ptr, ptr, -(block->mb_size));
            free(ptr);
            pthread_mutex_unlock(&mem_lock);
            return new_ptr;
        }
        else if(size <= old_size + next_block->mb_size + MB_STRUCT_RSIZE)
        {
            // can resize
            // join next block
            block->mb_size = -(old_size + next_block->mb_size + MB_STRUCT_RSIZE);
            LIST_REMOVE(next_block, mb_free_list);
            LIST_REMOVE(next_block, mb_list);
            reduce_block(arena, block, size);
        }
    }
    pthread_mutex_unlock(&mem_lock);
    return ptr;
}
