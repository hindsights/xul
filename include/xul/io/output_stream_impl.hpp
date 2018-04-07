#pragma once

#include <xul/io/output_stream.hpp>
#include <xul/lang/object_impl.hpp>
#include <assert.h>


namespace xul {


/// common output stream implementation
class output_stream_impl : public object_impl<output_stream>
{
public:
    output_stream_impl() : m_written_size(0)
    {
    }

    virtual void write_bytes( const uint8_t* data, int size )
    {
        do_write_bytes(data, size);
        on_bytes_written(size);
    }

    virtual void write_byte( uint8_t val )
    {
        do_write_byte(val);
        on_bytes_written(1);
    }
    virtual void write_string( const char* s )
    {
        if (nullptr == s)
            return;
        write_bytes(reinterpret_cast<const uint8_t*>(s), strlen(s));
    }

    virtual void write_chars( const char* data, int size )
    {
        if (nullptr == data || size <= 0)
            return;
        write_bytes(reinterpret_cast<const uint8_t*>(data), size);
    }

    virtual void write_char( char val )
    {
        write_byte(static_cast<uint8_t>(val));
    }
    
    virtual int64_t position() const
    {
        return m_written_size;
    }

    virtual bool seek(int64_t pos)
    {
        assert(false);
        return false;
    }

protected:
    virtual void on_bytes_written(int size)
    {
        m_written_size += size;
    }

    virtual void do_write_bytes( const uint8_t* data, int size ) = 0;

    virtual void do_write_byte( uint8_t val ) = 0;

protected:
    int64_t m_written_size;
};

    
}
