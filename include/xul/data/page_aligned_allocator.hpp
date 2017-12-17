#pragma once


#ifdef XUL_WINDOWS
#include <xul/mswin/windows.hpp>
#else
#include <stdlib.h>
#include <unistd.h> // _SC_PAGESIZE
#endif

#if defined(XUL_LINUX)
#include <malloc.h> // memalign
#endif

#ifdef ACCA_DEBUG_BUFFERS
#include <sys/mman.h>
#include "libacca/size_type.hpp"

struct alloc_header
{
    libacca::size_type size;
    int magic;
};

#endif


namespace xul {


class page_aligned_allocator
{
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static char* malloc(const size_type bytes)
    {
#ifdef ACCA_DEBUG_BUFFERS
        int page = page_size();
        char* ret = (char*)valloc(bytes + 2 * page);
        // make the two surrounding pages non-readable and -writable
        ACCA_ASSERT((bytes & (page-1)) == 0);
        alloc_header* h = (alloc_header*)ret;
        h->size = bytes;
        h->magic = 0x1337;
        mprotect(ret, page, PROT_READ);
        mprotect(ret + page + bytes, page, PROT_READ);
        //        fprintf(stderr, "malloc: %p head: %p tail: %p size: %d\n", ret + page, ret, ret + page + bytes, int(bytes));

        return ret + page;
#endif

#if defined XUL_WINDOWS
        return reinterpret_cast<char*>(VirtualAlloc(0, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
#else
        return reinterpret_cast<char*>(valloc(bytes));
#endif
    }

    static void free(char* const block)
    {

#ifdef ACCA_DEBUG_BUFFERS
        int page = page_size();
        // make the two surrounding pages non-readable and -writable
        mprotect(block - page, page, PROT_READ | PROT_WRITE);
        alloc_header* h = (alloc_header*)(block - page);
        ACCA_ASSERT((h->size & (page-1)) == 0);
        ACCA_ASSERT(h->magic == 0x1337);
        mprotect(block + h->size, page, PROT_READ | PROT_WRITE);
        //        fprintf(stderr, "free: %p head: %p tail: %p size: %d\n", block, block - page, block + h->size, int(h->size));
        h->magic = 0;

        ::free(block - page);
        return;
#endif

#if defined XUL_WINDOWS
        VirtualFree(block, 0, MEM_RELEASE);
#else
        ::free(block);
#endif
    }
};


}
