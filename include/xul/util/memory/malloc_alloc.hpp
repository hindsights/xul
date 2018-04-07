#pragma once

#include <utility>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


class malloc_alloc
{
public:
#if defined(_DEBUG) || !defined(NDEBUG)
    static void* allocate(size_t size, const char* filename = NULL, int line = 0)
#else
    static void* allocate(size_t size)
#endif
    {
        assert(size < 33 * 1024 * 1024);
        void* p = malloc(size);
        assert(p);
        return p;
    }

#if defined(_DEBUG) || !defined(NDEBUG)
    static void deallocate(void* p, size_t size = 0, const char* filename = NULL, int line = 0)
#else
    static void deallocate(void* p, size_t size = 0)
#endif
    {
        //assert(p != NULL);
        free(p);
    }
};

