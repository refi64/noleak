#include <noleak.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct _noleak_manager_t* _noleak_manager = NULL;

void _noleak_end()
{
    if (_noleak_manager == NULL) return;
    int leak_count=0;
    struct _noleak_node* cur = _noleak_manager->begin;
    struct _noleak_node* next = NULL;
    while (cur != NULL)
    {
        printf("Leak #%d at %s:", ++leak_count, cur->file);
        if (cur->func != NULL)
            printf("%s:", cur->func);
        printf("%d\n", cur->lineno);
        free(cur->file);
        free(cur->func);
        next = cur->next;
        free(cur);
        cur = next;
    }
    if (leak_count)
        printf("Total leaks: %d\n", leak_count);
    free(_noleak_manager);
    _noleak_manager = NULL;
}

void noleak_init()
{
    if (_noleak_manager != NULL) return;
    _noleak_manager = malloc(sizeof(struct _noleak_manager_t));
    _noleak_manager->begin = NULL;
    atexit(_noleak_end);
}

void* _noleak_add_mem(size_t b, const char* file, const char* func, int lineno)
{
    struct _noleak_node* orig_begin = _noleak_manager->begin;
    _noleak_manager->begin = malloc(sizeof(struct _noleak_node));
    if (_noleak_manager->begin == NULL)
        return NULL;
    _noleak_manager->begin->next = orig_begin;
    _noleak_manager->begin->mem = malloc(b);
    if (_noleak_manager->begin->mem == NULL)
    {
        free(_noleak_manager->begin);
        return NULL;
    }
    _noleak_manager->begin->file = malloc(strlen(file));
    if (_noleak_manager->begin->file == NULL)
    {
        free(_noleak_manager->begin->mem);
        free(_noleak_manager->begin);
        return NULL;
    }
    strcpy(_noleak_manager->begin->file, file);
    if (func == NULL)
        _noleak_manager->begin->func = NULL;
    else
    {
        _noleak_manager->begin->func = malloc(strlen(func));
        if (_noleak_manager->begin->func == NULL)
        {
            free(_noleak_manager->begin->file);
            free(_noleak_manager->begin->mem);
            free(_noleak_manager->begin);
            return NULL;
        }
        strcpy(_noleak_manager->begin->func, func);
    }
    _noleak_manager->begin->lineno = lineno-1;
    return _noleak_manager->begin->mem;
}

void* _noleak_realloc(void* mem, size_t b)
{
    struct _noleak_node* cur = _noleak_manager->begin;
    while (cur != NULL)
    {
        if (cur->mem == mem)
        {
            cur->mem = realloc(mem, b);
            return cur->mem;
        }
        else
            cur = cur->next;
    }
    printf("warning: untracked memory: %p\n", mem);
    return realloc(mem, b);
}

void _noleak_free(void* mem)
{
    struct _noleak_node* cur = _noleak_manager->begin;
    struct _noleak_node** prev = NULL;
    while (cur != NULL)
    {
        if (cur->mem == mem)
        {
            free(cur->file);
            free(cur->func);
            if (prev == NULL)
                _noleak_manager->begin = NULL;
            else
                (*prev)->next = cur->next;
            free(cur);
            break;
        }
        else
        {
            if (prev == NULL)
                prev = &_noleak_manager->begin;
            else
                prev = &((*prev)->next);
            cur = cur->next;
        }
    }
    free(mem);
}
