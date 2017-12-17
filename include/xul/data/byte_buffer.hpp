#pragma once

#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/noncopyable.hpp>
#include <algorithm>
#include <memory>
#include <stdint.h>
#include <assert.h>


namespace xul {


class byte_buffer : public object_impl<buffer>
{
#ifdef _DEBUG
private:
    static boost::detail::atomic_count& ref_total_buffer_count()
    {
        static boost::detail::atomic_count total_buffer_count(0);
        return total_buffer_count;
    }
public:
    static int get_total_buffer_count() { return ref_total_buffer_count(); }
#endif

public:
    enum { max_buffer_size = 32 * 1024 * 1024 + 1 };

    byte_buffer() : m_data(NULL), m_capacity(0), m_size(0)
    {
#ifdef _DEBUG
        ++ref_total_buffer_count();
#endif
    }
    explicit byte_buffer(const element_type* data, size_t size) : m_capacity(size), m_size(size)
    {
#ifdef _DEBUG
        ++ref_total_buffer_count();
#endif
        if ( size > 0 )
        {
            m_data = do_allocate(size);
            memcpy( m_data, data, size );
        }
        else
        {
            m_data = NULL;
        }
    }
    explicit byte_buffer(size_t size) : m_capacity(size), m_size(size)
    {
#ifdef _DEBUG
        ++ref_total_buffer_count();
#endif
        if ( size > 0 )
        {
            m_data = do_allocate(size);
        }
        else
        {
            m_data = NULL;
        }
    }
    explicit byte_buffer(size_t size, const element_type& initialVal) : m_capacity(size), m_size(size)
    {
#ifdef _DEBUG
        ++ref_total_buffer_count();
#endif
        if ( m_size > 0 )
        {
            m_data = do_allocate(size);
            std::fill_n( m_data, m_size, initialVal );
        }
        else
        {
            m_data = NULL;
        }
    }
    ~byte_buffer()
    {
#ifdef _DEBUG
        --ref_total_buffer_count();
#endif
        XUL_APP_DEBUG("buffer.delete " << xul::make_tuple(this, static_cast<void*>(m_data), m_capacity, m_size));
        if (m_data != NULL)
        {
            do_deallocate(m_data);
            m_data = NULL;
            m_capacity = 0;
            m_size = 0;
        }
        else
        {
            assert(m_capacity == 0 && m_size == 0);
        }
    }

    void reserve(size_t size)
    {
        assert(size < max_buffer_size);
        assert(m_size <= m_capacity);
        if (m_capacity >= size)
            return;
        byte_buffer newBuffer(size);
        newBuffer.resize(m_size);
        if (m_size > 0)
        {
            memcpy(newBuffer.m_data, m_data, m_size);
        }
        this->swap(newBuffer);
    }

    void resize(size_t size)
    {
        reserve(size);
        assert(m_capacity >= size);
        m_size = size;
    }
    void ensure_size(size_t size)
    {
        resize(0);
        resize(size);
    }

    void assign( const element_type& elem )
    {
        if ( m_data && m_size > 0 )
        {
            std::fill_n( m_data, m_size, elem );
        }
    }

    void assign( const buffer& src )
    {
        this->assign( src.data(), src.size() );
    }

    void assign(const element_type* src, size_t size)
    {
        if (size == 0)
        {
            this->resize(0);
            return;
        }
        assert(src != NULL && size > 0);
        //assert(!::IsBadReadPtr(src, size));
        resize(size);
        memcpy(m_data, src, size);
        assert(m_size == size);
        assert(m_capacity >= size);
    }

    void append(const element_type* src, size_t size)
    {
        size_t oldSize = m_size;
        resize(oldSize + size);
        memcpy(m_data + oldSize, src, size * sizeof(element_type));
    }

    void append(size_t size, const element_type& elem)
    {
        size_t oldSize = m_size;
        resize(oldSize + size);
        std::fill_n(m_data + oldSize, elem, size);
    }

    void append(const buffer& src)
    {
        this->append(src.data(), src.size());
    }

    void clear()
    {
        byte_buffer buf;
        this->swap(buf);
    }

    void swap(buffer& b)
    {
        byte_buffer* dst = static_cast<byte_buffer*>(&b);
        if (this == dst)
            return;
        std::swap(m_data, dst->m_data);
        std::swap(m_capacity, dst->m_capacity);
        std::swap(m_size, dst->m_size);
    }

    size_t size() const { return m_size; }

    size_t capacity() const { return m_capacity; }

    bool empty() const { return m_size == 0; }

    element_type* data() { return m_data; }

    const element_type* data() const { return m_data; }


    element_type* begin() { return m_data; }
    const element_type* begin() const { return m_data; }
    element_type* end() { return m_data + m_size; }
    const element_type* end() const { return m_data + m_size; }

    element_type operator[](size_t index) const { assert(!empty()); assert(index < m_size); return m_data[index]; }
    element_type& operator[](size_t index) { assert(!empty()); assert(index < m_size); return m_data[index]; }

private:
    static uint8_t* do_allocate(size_t size)
    {
        uint8_t* buf = static_cast<uint8_t*>(::malloc(size));
        assert(buf);
        return buf;
    }
    static void do_deallocate(uint8_t* buf)
    {
        assert(buf);
        ::free(buf);
    }

private:
    element_type* m_data;

    size_t m_capacity;

    size_t m_size;
};


inline void swap(byte_buffer & a, byte_buffer& b) // never throws
{
    a.swap(b);
}


inline buffer* create_buffer()
{
    return new byte_buffer;
}


}

