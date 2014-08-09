#ifndef NOLEAK_H
#define NOLEAK_H

#include <string.h>

#if defined(__cplusplus) || defined(NOLEAK_CPP_OVERRIDE)
#define _NOLEAK_CPP
#endif

#ifdef _NOLEAK_CPP
extern "C" {
#endif

#if __STDC_VERSION__ > 199901L || __cplusplus > 201103L
 #define _NOLEAK_FUNC __func__
#else
 #if __GNUC__ >= 2
  #define _NOLEAK_FUNC __FUNCTION__
 #else
  #define _NOLEAK_FUNC NULL
 #endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct _noleak_node
{
    struct _noleak_node* next;
    void* mem;
    char* file;
    char* func;
    int lineno;
};

struct _noleak_manager_t
{
    struct _noleak_node* begin;
};

void noleak_init();
void _noleak_end();

void* _noleak_add_mem(size_t b, const char* file, const char* func, int lineno);

void _noleak_free(void* mem);

#define NL_MALLOC(b) _noleak_add_mem(b, __FILE__, _NOLEAK_FUNC, __LINE__)
#define NL_REALLOC(p, b) _noleak_realloc(p, b)
#define NL_FREE(p) _noleak_free(p)

#ifdef _NOLEAK_CPP

}

#include <stdexcept>

namespace _noleak
{
    struct pos_info
    {
        explicit pos_info(const char* file, const char* func, const int lineno):
            file(file), func(func), lineno(lineno) {}
        const char* file;
        const char* func;
        const int lineno;
    };

    class mem_manager
    {
    public:
        mem_manager() { noleak_init(); }
        ~mem_manager() { _noleak_end(); }
    };

    static mem_manager mgr;
}

#define NOLEAK ::_noleak::pos_info(__FILE__, _NOLEAK_FUNC, __LINE__)

void* operator new(size_t b, const _noleak::pos_info& pos)
{
    void* mem = _noleak_add_mem(b, pos.file, pos.func, pos.lineno);
    if (mem == NULL)
        throw std::bad_alloc();
    return mem;
}

void operator delete(void* p) throw()
{
    NL_FREE(p);
}

#endif

#endif // NOLEAK_H
