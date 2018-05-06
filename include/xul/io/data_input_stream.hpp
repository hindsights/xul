#pragma once

#include <xul/io/memory_input_stream.hpp>
#include <xul/io/input_stream.hpp>
#include <xul/io/serializable.hpp>
#include <xul/data/byte_order.hpp>
#include <xul/data/buffer.hpp>

#include <boost/noncopyable.hpp>

#include <vector>
#include <string>
#include <assert.h>

#include <stdint.h>


namespace xul {


class data_input_stream : private boost::noncopyable, public input_stream
{
public:
    explicit data_input_stream( input_stream* is, bool bigEndian )
        : m_in( is )
        , m_byte_order( bigEndian )
        , m_good( is != NULL )
        , m_bit_offset(0)
    {
    }

    typedef bool data_input_stream::*unspecified_bool_type;

    operator unspecified_bool_type() const
    {
        return m_good ? &data_input_stream::m_good : 0;
    }

    bool operator!() const
    {
        return ! m_good;
    }

    bool good() const
    {
        return m_good;
    }
    void set_bad()
    {
        //assert(m_good);
        m_good = false;
    }
    byte_order& get_byte_order()
    {
        return m_byte_order;
    }

    void attach( input_stream& is )
    {
        m_in = &is;
        m_good = true;
    }

    input_stream& get_input() const
    {
        return *m_in;
    }
    virtual const uint8_t* get_buffer() const { return m_in->get_buffer(); }

    bool is_big_endian() const
    {
        return m_byte_order.is_big_endian();
    }
    void set_big_endian( bool isBigEndian )
    {
        m_byte_order.set_big_endian( isBigEndian );
    }

    int64_t available() const
    {
        if ( false == m_good )
            return 0;
        return m_in->available();
    }
    bool is_available( int64_t size ) const
    {
        if ( false == m_good )
            return false;
        return m_in->is_available( size );
    }

    bool skip( int64_t size )
    {
        if ( false == m_good )
            return false;
        if (0 == size)
            return true;
        m_good = m_in->skip( size );
        m_bit_offset = 0;
        return m_good;
    }

    int64_t position() const
    {
        return m_in->position();
    }
    bool seek(int64_t pos)
    {
        if (false == m_in->seek(pos))
            return false;
        m_good = true;
        m_bit_offset = 0;
        return true;
    }

    int64_t size() const
    {
        return m_in->size();
    }

    bool try_read_bytes( size_t size )
    {
        if ( is_available( size ) )
            return true;
        m_good = false;
        assert(false);
        return false;
    }

    int read_byte()
    {
        if ( false == m_good )
            return -1;
        int ch = m_in->read_byte();
        if ( ch < 0 )
        {
            set_bad();
            return -1;
        }
        m_bit_offset = 0;
        m_good = (ch >= 0);
        return ch;
    }

    virtual int peek_byte()
    {
        if (!m_good)
            return -1;
        return m_in->peek_byte();
    }


    bool read_byte( unsigned char& val )
    {
        if ( false == m_good )
            return false;
        int ch = m_in->read_byte();
        m_bit_offset = 0;
        m_good = (ch >= 0);
        val = ch;
        return m_good;
    }
    size_t read( uint8_t* buf, size_t size )
    {
        if ( false == m_good || 0 == size )
            return 0;
        size_t len = m_in->read( buf, size );
        if (0 == len)
        {
            m_good = false;
            return 0;
        }
        m_bit_offset = 0;
        return len;
    }
    bool read_n( uint8_t* buf, size_t size )
    {
        if ( false == m_good )
            return false;
        if (0 == size)
            return true;
        m_good = m_in->read_n( buf, size );
        m_bit_offset = 0;
        return m_good;
    }

    bool read_bytes( uint8_t* buf, size_t size )
    {
        return read_n(buf, size);
    }
    bool read_memory( void* buf, size_t size )
    {
        return read_n(reinterpret_cast<uint8_t*>(buf), size);
    }

    bool read_raw_buffer( unsigned char* buf, size_t size )
    {
        return read_bytes( buf, size );
    }
    bool read_raw_buffer( char* buf, size_t size )
    {
        return read_memory( buf, size );
    }

    template <typename T>
    bool read_array( T* buf, size_t size )
    {
        for ( size_t index = 0; index < size; ++index )
        {
            if ( *this >> buf[index] )
            {
                continue;
            }
            else
            {
                assert(false);
                return false;
            }
        }
        return true;
    }
    bool read_array( unsigned char* buf, size_t size )
    {
        return read_bytes( buf, size );
    }
    bool read_array( char* buf, size_t size )
    {
        return read_memory( buf, size );
    }

    bool read_buffer(byte_buffer& s, size_t count)
    {
        if ( false == try_read_bytes( count ) )
            return false;
        if ( 0 == count )
        {
            s.clear();
            return true;
        }
        s.resize( count );
        if ( read_raw_buffer( s.data(), count ) )
            return true;
        assert(false);
        return false;
    }

    template<typename TraitsT, typename AllocT>
    bool read_string(std::basic_string<char, TraitsT, AllocT>& s, size_t count)
    {
        if ( false == try_read_bytes( count ) )
            return false;
        if ( 0 == count )
        {
            s.erase();
            return true;
        }
        s.resize( count );
        if ( read_raw_buffer( &s[0], count ) )
            return true;
        assert(false);
        return false;
    }

    template<typename T, typename AllocT>
    bool read_vector(std::vector<T, AllocT>& items, size_t itemSize, size_t count)
    {
        if (0 == itemSize)
        {
            items.resize(0);
            for (size_t index = 0; index < count; ++index)
            {
                T obj;
                if (*this >> obj)
                {
                    items.push_back(obj);
                }
                else
                {
                    return false;
                }
            }
            return *this;
        }
        if ( false == try_read_bytes( count * itemSize ) )
            return false;
        if ( 0 == count )
        {
            items.clear();
            return true;
        }
        items.resize( count );
        return read_array( &items[0], count );
    }


    bool read_uint8( uint8_t& val )
    {
        return read_byte( val );
    }

    bool read_uint16( uint16_t& val )
    {
        unsigned short x;
        if ( false == read_memory( &x, 2 ) )
            return false;
        val = m_byte_order.convert_word( x );
        return true;
    }

    bool read_uint32( uint32_t& val )
    {
        unsigned int x;
        if ( false == read_memory( &x, 4 ) )
            return false;
        val = m_byte_order.convert_dword( x );
        return true;
    }

    bool read_uint64( uint64_t& val )
    {
        uint64_t x;
        if ( false == read_memory( &x, 8 ) )
            return false;
        val = m_byte_order.convert_qword( x );
        return true;
    }

    bool read_double( double& val )
    {
        double x;
        if ( false == read_memory( &x, 8 ) )
            return false;
        val = m_byte_order.convert_double( x );
        return true;
    }

    bool read_int8( signed char& val )
    {
        return read_uint8( reinterpret_cast<uint8_t&>( val ) );
    }

    bool read_int16( int16_t& val )
    {
        return read_uint16( reinterpret_cast<uint16_t&>( val ) );
    }

    bool read_int32( int32_t& val )
    {
        return read_uint32( reinterpret_cast<uint32_t&>( val ) );
    }

    bool read_int64( int64_t& val )
    {
        return read_uint64( reinterpret_cast<uint64_t&>( val ) );
    }
    bool read_bool( bool& val )
    {
        uint8_t byteval;
        if (!read_byte(byteval))
        {
            return false;
        }
        val = (byteval != 0);
        return true;
    }

private:
    input_stream* m_in;
    byte_order m_byte_order;
    bool m_good;
    unsigned m_bit_offset;
};


inline data_input_stream& operator>>( data_input_stream& is, signed char& val )
{
    is.read_int8( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, char& val )
{
    is.read_int8( reinterpret_cast<signed char&>( val ) );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, unsigned char& val )
{
    is.read_uint8( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, signed short& val )
{
    is.read_int16( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, unsigned short& val )
{
    is.read_uint16( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, int& val )
{
    is.read_int32( reinterpret_cast<int&>( val ) );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, unsigned int& val )
{
    is.read_uint32( reinterpret_cast<unsigned int&>( val ) );
    return is;
}

#if !defined(__GNUC__) || !defined(__x86_64__)
// non-gcc or gcc with non-64bit mode
inline data_input_stream& operator>>( data_input_stream& is, long& val )
{
    is.read_int32( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, unsigned long& val )
{
    is.read_uint32( val );
    return is;
}
#endif

inline data_input_stream& operator>>( data_input_stream& is, int64_t& val )
{
    is.read_int64( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, uint64_t& val )
{
    is.read_uint64( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, bool& val )
{
    is.read_bool( val );
    return is;
}

inline data_input_stream& operator>>( data_input_stream& is, serializable& obj )
{
    if ( false == obj.read_object( is ) )
    {
        is.set_bad();
    }
    return is;
}


class memory_data_input_stream : public data_input_stream
{
public:
    memory_data_input_stream( const void* data, size_t size, bool bigEndian ) : data_input_stream( NULL, bigEndian ), m_mis( data, size )
    {
        this->attach( m_mis );
    }

private:
    memory_input_stream m_mis;
};


}
