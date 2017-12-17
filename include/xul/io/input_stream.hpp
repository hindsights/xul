#pragma once

#include <xul/io/input_stream_callback.hpp>
#include <xul/data/buffer.hpp>
#include <stddef.h>
#include <stdint.h>

namespace xul {


class input_stream
{
public:
    input_stream() { }
    virtual ~input_stream() { }

    virtual const uint8_t* get_buffer() const { return NULL; }

    virtual bool is_open() const { return false; }

    virtual void close() { }

    virtual bool read_n( uint8_t* buf, size_t size )
    {
        return read(buf, size) == size;
    }
    virtual bool read_memory( void* buf, size_t size )
    {
        return read(static_cast<uint8_t*>(buf), size) == size;
    }

    virtual void async_read(size_t size, input_stream_callback* callback)
    {
        byte_buffer buf;
        buf.resize(size);
        size_t readSize = read(buf.data(), size);
        if (0 == readSize)
        {
            callback->on_data_read(this, -1, NULL, 0, -1);
            return;
        }
        callback->on_data_read(this, 0, buf.data(), readSize, -1);
    }
    virtual void async_read_n(size_t size, input_stream_callback* callback)
    {
        byte_buffer buf;
        buf.resize(size);
        if (false == read_n(buf.data(), size))
        {
            callback->on_data_read(this, -1, NULL, 0, -1);
            return;
        }
        callback->on_data_read(this, 0, buf.data(), size, -1);
    }

    virtual size_t read(uint8_t* buf, size_t size) = 0;

    virtual int read_byte() = 0;
    virtual int peek_byte() = 0;

    virtual int64_t available() const = 0;

    virtual bool is_available( int64_t size ) const
    {
        return this->available() >= size;
    }

    virtual bool skip( int64_t size ) = 0;

    //virtual bool unread(size_t size) = 0;

    virtual int64_t position() const = 0;

    virtual bool seek(int64_t pos) = 0;
    virtual bool seek_slice(int piece, int pos) { return false; }
#if 0
    virtual bool seek(int64_t pos, int64_t p2p_offset)
    {
        return true;
    }
#endif

    virtual int64_t size() const = 0;

    //virtual const unsigned char* get_buffer() = 0;
};


}
