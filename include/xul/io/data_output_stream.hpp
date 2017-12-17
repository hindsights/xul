
#pragma once

#include <xul/io/output_stream.hpp>
#include <xul/io/memory_output_stream.hpp>
#include <xul/io/ostream_stream.hpp>
#include <xul/io/serializable.hpp>
#include <xul/data/byte_order.hpp>
#include <xul/data/buffer.hpp>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <assert.h>
#include <stdint.h>


namespace xul {


class data_output_stream : private boost::noncopyable
{
public:
    explicit data_output_stream( output_stream* os, bool bigEndian )
        : m_out( os )
        , m_byte_order( bigEndian )//, m_good( true )
    {
    }

    void attach( output_stream& out )
    {
        m_out = &out;
    }

    output_stream& get_output() const
    {
        return *m_out;
    }

    bool is_big_endian() const
    {
        return m_byte_order.is_big_endian();
    }
    void set_big_endian( bool isBigEndian )
    {
        m_byte_order.set_big_endian( isBigEndian );
    }

    int64_t position() const
    {
        return m_out->position();
    }
    bool seek(int64_t pos)
    {
        if (!m_out->seek(pos))
            return false;
        return true;
    }

    //typedef bool data_output_stream::*unspecified_bool_type;

    //operator unspecified_bool_type() const
    //{
    //    return m_good ? &data_output_stream::m_good : 0;
    //}


    void write_byte( unsigned char val )
    {
        m_out->write_byte( val );
    }

    void write_bytes( const void* buf, size_t size )
    {
        m_out->write_bytes( static_cast<const uint8_t*>(buf), size );
    }

    template<typename CharT, typename TraitsT, typename AllocT>
    void write_string( const std::basic_string<CharT, TraitsT, AllocT>& s )
    {
        this->write_raw_buffer( s.data(), s.size() );
    }

    void write_buffer( const buffer& s )
    {
        this->write_raw_buffer( s.data(), s.size() );
    }

    template<typename T, typename AllocT>
    void write_vector( const std::vector<T, AllocT>& items )
    {
        if ( items.size() > 0 )
        {
            write_array( &items[0], items.size() );
        }
    }


    void write_uint8( unsigned char val )
    {
        write_byte( val );
    }

    void write_uint16( unsigned short val )
    {
        uint16_t x = m_byte_order.convert_word( val );
        write_bytes( &x, 2 );
    }

    void write_uint32( unsigned int val )
    {
        uint32_t x = m_byte_order.convert_dword( val );
        write_bytes( &x, 4 );
    }

    void write_uint64( uint64_t val )
    {
        uint64_t x = m_byte_order.convert_qword( val );
        write_bytes( &x, 8 );
    }

    void write_int8( signed char val )
    {
        write_uint8( static_cast<unsigned char>( val ) );
    }

    void write_int16( short val )
    {
        write_uint16( static_cast<unsigned short>( val ) );
    }

    void write_int32( int val )
    {
        write_uint32( static_cast<unsigned int>( val ) );
    }

    void write_int64( int64_t val )
    {
        write_uint64( static_cast<uint64_t>( val ) );
    }

    void write_double(double val)
    {
        double x = m_byte_order.convert_double( val );
        write_bytes( &x, 8 );
    }

    template <typename T>
    void write_array( const T* data, size_t size )
    {
        for ( size_t index = 0; index < size; ++index )
        {
            *this << data[index];
        }
    }
    void write_array( const unsigned char* data, size_t size )
    {
        this->write_bytes( data, size );
    }
    void write_array( const char* data, size_t size )
    {
        this->write_bytes( data, size );
    }

    void write_raw_buffer( const unsigned char* data, size_t size )
    {
        this->write_bytes( data, size );
    }
    void write_raw_buffer( const char* data, size_t size )
    {
        this->write_bytes( data, size );
    }

private:
    output_stream* m_out;
    byte_order m_byte_order;
    //bool m_good;
};


inline data_output_stream& operator<<( data_output_stream& os, signed char val )
{
    os.write_int8( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, char val )
{
    os.write_int8( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, unsigned char val )
{
    os.write_uint8( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, signed short val )
{
    os.write_int16( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, unsigned short val )
{
    os.write_uint16( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, int val )
{
    os.write_int32( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, unsigned int val )
{
    os.write_uint32( val );
    return os;
}

#if !defined(__GNUC__) || !defined(__x86_64__)
// non-gcc or gcc with non-64bit mode
inline data_output_stream& operator<<( data_output_stream& os, long val )
{
    os.write_int32( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, unsigned long val )
{
    os.write_uint32( val );
    return os;
}
#endif
inline data_output_stream& operator<<( data_output_stream& os, int64_t val )
{
    os.write_int64( val );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, uint64_t val )
{
    os.write_uint64( val );
    return os;
}

template<typename TraitsT, typename AllocT>
inline data_output_stream& operator<<( data_output_stream& os, const std::basic_string<char, TraitsT, AllocT>& s )
{
    os.write_string( s );
    return os;
}

template<typename TraitsT, typename AllocT>
inline data_output_stream& operator<<( data_output_stream& os, const std::basic_string<unsigned char, TraitsT, AllocT>& s )
{
    os.write_string( s );
    return os;
}

template <typename T, typename AllocT>
inline data_output_stream& operator<<( data_output_stream& os, const std::vector<T, AllocT>& data )
{
    os.write_vector( data );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, const buffer& data )
{
    os.write_buffer( data );
    return os;
}

inline data_output_stream& operator<<( data_output_stream& os, const serializable& obj )
{
    size_t oldPos = os.position();
    obj.write_object( os );
    //assert(obj.get_object_size() < 0 || obj.get_object_size() + oldPos == os.position());
    return os;
}


class memory_data_output_stream : public data_output_stream
{
public:
    memory_data_output_stream( void* data, size_t size, bool bigEndian ) : data_output_stream( NULL, bigEndian ), m_mis( data, size )
    {
        this->attach( m_mis );
    }

private:
    memory_output_stream m_mis;
};

    
class ostream_data_output_stream : public data_output_stream
{
public:
    ostream_data_output_stream( std::ostream& os, bool bigEndian ) : data_output_stream( NULL, bigEndian ), m_mis( os )
    {
        this->attach( m_mis );
    }
    
private:
    ostream_stream m_mis;
};


}
