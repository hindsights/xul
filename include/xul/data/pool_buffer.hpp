#pragma once

#include <xul/data/pool.hpp>
#include <xul/lang/object_impl.hpp>
#include <stdint.h>


namespace xul {


class pool;
class pool_buffer;


class pool_buffer : public object_impl<object>
{
public:
    explicit pool_buffer(pool& p, char* buf, size_t size)
        : m_pool(p)
        , m_buffer(buf)
        , m_size(size)
    {
        assert((NULL == buf && 0 == size) || check_valid());
    }
    ~pool_buffer()
    {
        reset();
    }

    bool is_valid() const { return m_buffer != NULL; }
    bool check_valid() const
    {
        if (NULL == m_buffer)
            return false;
        assert(m_size > 0);
        return m_pool.is_from(m_buffer, m_size);
    }

    //char* get() { return m_buffer; }
    char* get_buffer() { return (char*)m_buffer; }
    pool* get_pool() { return &m_pool; }
    size_t get_size() const { return m_size; }

    char* release()
    {
        char* buf = m_buffer;
        m_buffer = NULL;
        return buf;
    }

    void reset(char* buf = NULL, size_t size = 0)
    {
        if (NULL != m_buffer)
        {
            assert(m_size > 0);
            m_pool.deallocate(m_buffer);
        }
        m_buffer = buf;
        m_size = size;
        assert(NULL == buf || m_pool.is_from(buf, size));
        assert(m_buffer != NULL || 0 == m_size);
    }

    typedef char* (pool_buffer::*unspecified_bool_type)();
    operator unspecified_bool_type() const
    { return m_buffer == 0? 0: &pool_buffer::release; }

private:
    pool& m_pool;
    char* m_buffer;
    size_t m_size;
};


typedef boost::intrusive_ptr<pool_buffer> pool_buffer_ptr;
typedef boost::intrusive_ptr<const pool_buffer> const_pool_buffer_ptr;

}
