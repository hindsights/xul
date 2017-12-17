#pragma once

#include <xul/io/input_stream.hpp>
#include <xul/macro/memory.hpp>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <assert.h>
#include <stdint.h>
#include <string.h>


namespace xul {


class memory_input_stream : public input_stream, private boost::noncopyable
{
public:
    explicit memory_input_stream(const void* buf, size_t size)
    {
        attach_buffer(buf, size);
    }
    memory_input_stream()
    {
        m_buffer = NULL;
        m_size = 0;
        m_position = 0;
    }

    virtual const uint8_t* get_buffer() const { return m_buffer; }
    void attach_buffer(const void* buf, size_t size)
    {
        assert(buf != NULL);
        m_buffer = static_cast<const unsigned char*>(buf);
        m_size = size;
        m_position = 0;
    }

    virtual size_t read(uint8_t* buf, size_t size)
    {
        if ( false == is_available(size) )
            return 0;
        if (size > 0)
        {
            memcpy(buf, m_buffer + m_position, size);
            m_position += size;
        }
        return size;
    }

    virtual int peek_byte()
    {
        if (m_position >= m_size)
            return -1;
        return m_buffer[m_position];
    }
    virtual int read_byte()
    {
        if ( false == is_available( 1 ) )
            return -1;
        int val = m_buffer[m_position];
        ++m_position;
        return val;
    }

    virtual int64_t available() const
    {
        if ( m_size >= m_position )
            return m_size - m_position;
        return 0;
    }
    virtual bool is_available( int64_t size ) const
    {
        return (m_position + size <= m_size);
    }

    virtual bool skip( int64_t size )
    {
        if ( false == is_available( size ) )
            return false;
        m_position += size;
        return true;
    }

    virtual int64_t position() const
    {
        return m_position;
    }
    virtual bool seek(int64_t pos)
    {
        if (pos > m_size)
            return false;
        m_position = pos;
        return true;
    }

    virtual int64_t size() const
    {
        return m_size;
    }

    //void check_available(size_t size)
    //{
    //    assert( is_available( size ) );
    //}

    template <typename ValueT>
    bool read_value(ValueT& val)
    {
        if ( false == is_available( sizeof(ValueT) ) )
            return false;
        val = XUL_READ_MEMORY(m_buffer + m_position, ValueT);
        m_position += sizeof(ValueT);
        return true;
    }

    template <typename ValueT>
    bool read_array(ValueT* vals, size_t count)
    {
        if ( 0 == count )
            return true;
        return read_memory(vals, sizeof(ValueT) * count);
    }

    template <typename ValueT, typename AllocatorT>
    bool read_vector(size_t count, std::vector<ValueT, AllocatorT>& vals)
    {
        if ( 0 == count )
            return true;
        if ( false == is_available( sizeof(ValueT) * count ) )
        {
            assert(false);
            return false;
        }
        vals.resize( count );
        if ( read_array( &vals[0], count ) )
            return true;
        vals.clear();
        return false;
    }

    template <typename StructT>
    bool read_struct(StructT& val)
    {
        return read_memory(&val, sizeof(StructT));
    }

    template <typename CharT, typename CharTraitsT, typename AllocatorT>
    bool read_string(size_t count, std::basic_string<CharT, CharTraitsT, AllocatorT>& s)
    {
        if ( 0 == count )
        {
            s.clear();
            return true;
        }
        if ( false == is_available( count * sizeof(CharT) ) )
        {
            assert(false);
            return false;
        }
        s.resize( count );
        if ( read_array( &s[0], count ) )
            return true;
        s.clear();
        return false;
    }

    template <typename ObjT>
    bool read_object( ObjT& obj )
    {
        return obj.read_object( *this );
    }

private:
    const unsigned char* m_buffer;
    size_t m_size;
    int64_t m_position;
};


}
