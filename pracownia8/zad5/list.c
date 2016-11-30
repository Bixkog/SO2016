// Wojciech Oziebly
#include "list.h"

node* node_init(int v, node* prev, node* next)
{
    node* e = (node*) malloc(sizeof(node));
    e->val = v;
    e->next = next;
    e->prev = prev;
    return e;
}

void list_init(list* l)
{
    l->begin = node_init(0, NULL, NULL); // setup end needed
    l->end = node_init(0, l->begin, NULL);
    l->begin->next = l->end;
    pthread_mutex_init(&(l->add_m), NULL);
    pthread_mutex_init(&(l->delete_m), NULL);
    pthread_mutex_init(&(l->search_m), NULL);
    pthread_cond_init(&(l->no_readers), NULL);
    l->readers = 0;
}

void list_destroy(list* l)
{
    node* t = l->begin;
    node* next;
    while(t != l->end)
    {
        next = t->next;
        free(t);
        t = next;
    }
    free(t);
    pthread_mutex_destroy(&(l->add_m));
    pthread_mutex_destroy(&(l->delete_m));                                                  
}

void add(list* l, int v)
{
    pthread_mutex_lock(&(l->add_m));
    
    node* new = node_init(v, l->end->prev, l->end);
    l->end->prev->next = new;
    l->end->prev = new;
    
    pthread_mutex_unlock(&(l->add_m));
}

void delete(list* l, int v)
{
    pthread_mutex_lock(&(l->delete_m));
    pthread_mutex_lock(&(l->search_m));
    while(l->readers > 0)
        pthread_cond_wait(&(l->no_readers), &(l->search_m));
    pthread_mutex_lock(&(l->add_m));
    
    node* it = l->begin->next;
    while(it != l->end && it->val != v)
    {
        it = it->next;
    }
    if(it != l->end)
    {
        it->prev->next = it->next;
        it->next->prev = it->prev;
        free(it);
    }

    pthread_mutex_unlock(&(l->add_m));
    pthread_mutex_unlock(&(l->search_m));
    pthread_mutex_unlock(&(l->delete_m));
}

node* search(list* l, int v)
{
    node* end;

    pthread_mutex_lock(&(l->search_m));
    l->readers++;
    pthread_mutex_unlock(&(l->search_m));
    
    pthread_mutex_lock(&(l->add_m));
    end = l->end;
    pthread_mutex_unlock(&(l->add_m));

    node* it = l->begin->next;
    while(it != end && it->val != v)
    {
        it = it->next;
    }
    
    pthread_mutex_lock(&(l->search_m));
    
    l->readers--;
    if(l->readers == 0)
        pthread_cond_signal(&(l->no_readers));
    
    pthread_mutex_unlock(&(l->search_m));
    
    if(it != l->end)
        return it;
    return NULL;
}
