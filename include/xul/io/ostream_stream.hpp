#pragma once

#include <xul/io/output_stream.hpp>
#include <xul/lang/object_impl.hpp>

#include <vector>
#include <string>
#include <ostream>
#include <stdexcept>
#include <assert.h>
#include <stdint.h>
#include <limits.h>


namespace xul {


class ostream_stream : public object_impl<output_stream>
{
public:
    explicit ostream_stream(std::ostream& os) : m_os(os)
    {
        m_start_position = os.tellp();
        if (m_start_position < 0)
            m_start_position = 0;
    }
    
    int capacity() const { return INT_MAX; }
    int64_t size() const
    {
        int64_t pos = m_os.tellp();
        m_os.seekp(0, std::ios_base::end);
        int64_t totalsize = m_os.tellp();
        m_os.seekp(pos);
        return totalsize;
    }
    
    void do_write_bytes(const void* data, int size)
    {
        write_chars(static_cast<const char*>(data), size);
    }
    
    virtual void write_bytes(const uint8_t* data, int size)
    {
        do_write_bytes(data, size);
    }
    
    virtual void write_byte( unsigned char val )
    {
        write_char(val);
    }
    
    virtual void write_char( char val )
    {
        m_os.write(&val, 1);
    }
    
    /// write a 0-terminated string
    virtual void write_string( const char* s )
    {
        write_chars(s, strlen(s));
    }
    
    /// write n chars buffer
    virtual void write_chars( const char* data, int size )
    {
        if (size > 0)
        {
            m_os.write(data, size);
        }
    }
    
    virtual int64_t position() const
    {
        return m_os.tellp();
    }
    virtual bool seek(int64_t pos)
    {
        if (pos > size())
        {
            assert(false);
            return false;
        }
        m_os.seekp(pos);
        return true;
    }
    
    template <typename ValueT>
    void write_value(ValueT val)
    {
        do_write_bytes(&val, sizeof(ValueT));
    }
    
    
    template <typename ValueT>
    void write_array(const ValueT* vals, int count)
    {
        do_write_bytes(vals, sizeof(ValueT) * count);
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
        do_write_bytes(&val, sizeof(StructT));
    }
    
    template <typename CharT, typename CharTraitsT, typename AllocatorT>
    void write_string(const std::basic_string<CharT, CharTraitsT, AllocatorT>& s)
    {
        do_write_bytes(s.data(), s.size() * sizeof( CharT ) );
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
    std::ostream& m_os;
    int64_t m_start_position;
};


}
