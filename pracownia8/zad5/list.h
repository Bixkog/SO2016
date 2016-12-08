// Wojciech Oziebly
#ifndef MY_LIST_H
#define MY_LIST_H

#include <pthread.h>
#include <stdlib.h>

struct node
{
    int val;
    struct node* next;
    struct node* prev;

};
typedef struct node node;

typedef struct
{
    node* begin;
    node* end;
    pthread_mutex_t add_m;
    pthread_mutex_t delete_m;
    pthread_mutex_t search_m;
    pthread_mutex_t draining_m;
    pthread_cond_t no_readers;
    pthread_cond_t drain_rest;
    size_t deleters;
    size_t readers;
} list;

void list_init(list* l);
void list_destroy(list* l);
void add(list* l, int v);
void delete(list* l, int v);
node* search(list* l, int v);

#endif
