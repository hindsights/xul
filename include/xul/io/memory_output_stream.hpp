#pragma once

#include <xul/io/output_stream.hpp>
#include <xul/macro/memory.hpp>
#include <xul/lang/object_impl.hpp>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <stdexcept>
#include <assert.h>
#include <string.h>
#include <stdint.h>


namespace xul {


class memory_output_stream : public object_impl<output_stream>
{
public:
    explicit memory_output_stream(void* buf, int capacity) : m_buffer(static_cast<unsigned char*>(buf)), m_capacity(capacity), m_position(0)
    {
        assert(m_buffer != NULL && m_capacity > 0);
        //assert( ! ::IsBadWritePtr(m_buffer, m_capacity));
    }

    int capacity() const { return m_capacity; }
    int64_t size() const { return m_position; }

    virtual void write_bytes(const uint8_t* data, int size)
    {
        check_available(size);
        if (size > 0)
        {
            memcpy(m_buffer + m_position, data, size);
            m_position += size;
        }
    }

    virtual void write_byte( unsigned char val )
    {
        check_available( 1 );
        m_buffer[m_position] = val;
        m_position += 1;
    }

    virtual void write_char( char val )
    {
        write_byte(val);
    }

    /// write a 0-terminated string
    virtual void write_string( const char* s )
    {
        write_bytes((const uint8_t*)s, strlen(s));
    }

    /// write n chars buffer
    virtual void write_chars( const char* data, int size )
    {
        write_bytes((const uint8_t*)data, size);
    }

    virtual int64_t position() const
    {
        return m_position;
    }
    virtual bool seek(int64_t pos)
    {
        if (pos >= m_capacity)
            return false;
        m_position = pos;
        return true;
    }

    void check_available(int size)
    {
        if ( false == this->is_available(size) )
            throw std::overflow_error("memory output stream overflow");
    }

    bool is_available(int size)
    {
        return (m_position + size <= m_capacity);
    }

    template <typename ValueT>
    void write_value(ValueT val)
    {
        check_available(sizeof(ValueT));
        XUL_WRITE_MEMORY(m_buffer + m_position, val, ValueT);
        m_position += sizeof(ValueT);
    }


    template <typename ValueT>
    void write_array(const ValueT* vals, int count)
    {
        write_bytes(vals, sizeof(ValueT) * count);
    }

    template <typename ValueT, typename AllocatorT>
    void write_vector(const std::vector<ValueT, AllocatorT>& vals)
    {
        if (!vals.empty())
        {
            write_array(&vals[0], vals.size());
        }
    }

    template <typename StructT>
    void write_struct(const StructT& val)
    {
        write_bytes(&val, sizeof(StructT));
    }

    template <typename CharT, typename CharTraitsT, typename AllocatorT>
    void write_string(const std::basic_string<CharT, CharTraitsT, AllocatorT>& s)
    {
        this->write_bytes( s.data(), s.size() * sizeof( CharT ) );
    }

    //void write_string(const char* s)
    //{
    //    this->write_bytes( s, strlen(s) );
    //}

    template <typename ObjT>
    void write_object( const ObjT& obj )
    {
        obj.write_object( *this );
    }

private:
    unsigned char* m_buffer;
    int m_capacity;
    int64_t m_position;
};



}


