#pragma once


namespace xul {

template <typename AllocT>
class debug_alloc
{
public:
    static const unsigned long MEM_TAG = 0X12345678;
    static const unsigned long MEM_CLEAR_TAG = 0X9ABCDEF0;

    static const unsigned long MEM_GUARD = 0xFAFAFAFA;
    static const unsigned char MEM_UNINITIALIZED = 0xFB;
    static const unsigned char MEM_FREED = 0xF9;

    void CheckMemGuard(void* p, size_t size)
    {
        assert(memchr(p, MEM_UNINITIALIZED, size) == 0);
    }

    struct debug_header
    {
        size_t size;
        unsigned long tag;
        unsigned long alloc_time;
        const char* filename;
        int line;
        unsigned long front_guard;
    };
    void clear()
    {
        m_impl.clear();
    }

    void* allocate(size_t size, const char* filename = NULL, int line = 0)
    {
        debug_header* header = static_cast<debug_header*>(m_impl.allocate(size + sizeof(debug_header) + 8));
        header->size = size;
        header->alloc_time = ::GetTickCount();
        header->filename = filename;
        header->line = line;
        assert(header->tag != MEM_TAG);
        header->tag = MEM_TAG;
        header->front_guard = MEM_GUARD;
        char* buf = reinterpret_cast<char*>(header + 1);
        memset(buf, MEM_UNINITIALIZED, size);
        XUL_WRITE_MEMORY(buf + size, MEM_GUARD, unsigned long);
        return buf;
    }
    void deallocate(void* p, const char* filename = NULL, int line = 0)
    {
        debug_header* header = static_cast<debug_header*>(p);
        --header;
        if (header->tag != MEM_TAG)
        {
            APP_EVENT("alloc: deallcate error " << p);
            assert(false);
            return;
        }
        assert(header->tag == MEM_TAG);
        assert(header->front_guard == MEM_GUARD);
        const char* buf = reinterpret_cast<const char*>(p);
        assert(XUL_READ_MEMORY(buf + header->size, unsigned long) == MEM_GUARD);
        header->tag = MEM_CLEAR_TAG;
        memset(p, MEM_FREED, header->size);
        m_impl.deallocate(header);
    }
    void print_status()
    {
        m_impl.print_status();
    }

private:
    AllocT m_impl;
};

}

