#include <sys/ueue.h>
#include <unistd.h>
#include <errno.h>
#include "mem_arena.h"

size_t DEFAULT_ARENA_SIZE = getpagesize()*4;
size_t PAGE_SIZE = getpagesize();

// add starting point of mmap
int create_arena(mem_arena_t** arena, size_t arena_size)
{
    if(arena_size == 0) arena_size = DEFAULT_ARENA_SIZE;
    *arena = mmap(NULL, 
                arena_size, 
                PROT_READ | PROT_WRITE,
                MEM_ANONYMOUS | MEM_PRIVATE,
                -1,
                0);
    if(*arena == MAP_FAILED) return 1;
    // initialize fields
    *arena->size = arena_size - sizeof(mem_arena_t);
    *arena->ma_first.mb_size = *arena->size;
    pthread_mutex_init(&(arena->ma_lock));
    // initialize list
    LIST_INIT(&(*arena->ma_freeblks));
    LIST_INSERT_HEAD(&(*arena->ma_freeblks), &(*arena->ma_first), mb_free_list);
    return 0;
}

int destroy_arena(mem_arena_t* arena)
{
    pthread_mutex_destroy(&(arena->ma_lock)); // needed?
    int err = munmap(arena, arena->size + sizeof(mem_arena_t));
    return err;
}

size_t calc_new_block_address(size_t start, size_t allocation_size)
{
    size_t new_block_address = start + sizeof(mem_block_t) // mem_block struct end
                               + allocation_size;
    if(new_block_address%8)
        new_block_address = ((new_block_address >> 3) + 1) << 3; // allign to 8
    return new_block_address;
}

void fill_chunk(mem_block_t* to_allocate, size_t allocation_size)
{
    // create new block
    uint64_t new_block_address = calc_new_block_address((size_t) to_allocate, allocation_size);
    mem_block_t* free_end = (mem_block_t*) new_block_address;
    free_end->mb_size = to_allocate->mb_size - allocation_size - sizeof(mem_block_t);
    // link new block and unlink to_allocate from free list
    LIST_INSERT_AFTER(to_allocate, free_end, mb_free_list); // add new free block
    LIST_INSERT_AFTER(to_allocate, free_end, mb_list); // add new block
    LIST_REMOVE(to_allocate, mb_free_list); // remove to_allocate from free list
    // allocate block
    to_allocate->mb_size = -((ssize_t) allocation_size);
}

void fill_whole(mem_block_t* to_allocate)
{
    LIST_REMOVE(to_allocate, mb_free_list);
    to_allocate->mb_size = -(to_allocate->mb_size);
}

void* allocate_block(mem_arena_t* arena, size_t allocation_size)
{
    pthread_mutex_lock(&(arena->ma_lock));
    mem_block_t* current_block;
    LIST_FOREACH(current_block, &(arena->ma_freeblks), mb_free_list)
    {
        if(current_block->mb_size >= allocation_size)
        {   
            if(current_block->mb_size >= allocation_size + sizeof(mem_block_t) + 8)
            {
                fill_chunk(current_block, allocation_size);
            }
            else
            {
                fill_whole(current_block);
            }
            pthread_mutex_unlock(&(arena->ma_lock));
            return (void*) &(current_block->mb_data);
        }
    }
    pthread_mutex_unlock(&(arena->ma_lock));
    return NULL;
}

void* allocate_big_block(size_t allocation_size)
{
    // calculate new arena size
    size_t arena_size = DEFAULT_ARENA_SIZE;
    while(arena_size < allocation_size)
        arena_size += PAGE_SIZE;
    // create arena
    mem_arena_t* new_arena;
    if(create_arena(&new_arena, arena_size) > 0)
        return NULL;
    // allocate at free arena
    allocate_block(new_arena, allocation_size);

    // connect arena after alloc
    pthread_mutex_lock(&arena_list_lock);
    LIST_INSERT_HEAD(&arena_list, new_arena, ma_list);
    pthread_mutex_unlock(&arena_list_lock);
}

// linear to count of arenas
// used in free and realloc
mem_arena_t* find_arena(uint64_t address)
{
    mem_arena_t* current_arena;
    LIST_FOREACH(current_arena, &arena_list, ma_list)
    {
        if((uint64_t) current_arena < address && 
           (uint64_t) current_arena + size > address)
            return current_arena;
    }
    return NULL;
}

