#pragma once

#include <xul/lang/object.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


/// interface for output stream
class output_stream : public object
{
public:
    /// write n bytes buffer
    virtual void write_bytes( const uint8_t* data, int size ) = 0;

    /// write one byte
    virtual void write_byte( uint8_t val ) = 0;

    /// write a 0-terminated string
    virtual void write_string( const char* s ) = 0;

    /// write n chars buffer
    virtual void write_chars( const char* data, int size ) = 0;

    /// write one char
    virtual void write_char( char val ) = 0;

    virtual int64_t position() const = 0;

    virtual bool seek(int64_t pos) = 0;
};


}
