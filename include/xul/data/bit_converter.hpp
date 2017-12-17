#pragma once

#include <stdint.h>
#include <assert.h>


namespace xul {


class bit_converter
{
public:
    static uint16_t make_word( uint8_t high, uint8_t low )
    {
        return ( static_cast<uint16_t>( high ) << 8 ) | static_cast<uint16_t>( low );
    }
    static uint8_t high_byte( uint16_t x )
    {
        return static_cast<uint8_t>( x >> 8 );
    }
    static uint8_t low_byte( uint16_t x )
    {
        return static_cast<uint8_t>( x );
    }

    static uint32_t make_dword( uint16_t high, uint16_t low )
    {
        return ( static_cast<uint32_t>( high ) << 16 ) | static_cast<uint32_t>( low );
    }
    static uint16_t high_word( uint32_t x )
    {
        return static_cast<uint16_t>( x >> 16 );
    }
    static uint16_t low_word( uint32_t x )
    {
        return static_cast<uint16_t>( x );
    }

    static uint64_t make_qword( uint32_t high, uint32_t low )
    {
        return ( static_cast<uint64_t>( high ) << 32 ) | static_cast<uint64_t>( low );
    }
    static uint32_t high_dword( uint64_t x )
    {
        return static_cast<uint32_t>( x >> 32 );
    }
    static uint32_t low_dword( uint64_t x )
    {
        return static_cast<uint32_t>( x );
    }

    static bit_converter& big_endian()
    {
        static bit_converter converter( true );
        return converter;
    }
    static bit_converter& little_endian()
    {
        static bit_converter converter( false );
        return converter;
    }

    explicit bit_converter( bool isBigEndian ) : m_big_endian( isBigEndian )
    {
    }

    void set_big_endian( bool isBigEndian )
    {
        m_big_endian = isBigEndian;
    }
    bool is_big_endian() const
    {
        return m_big_endian;
    }

    uint16_t to_word( const char* data )
    {
        return to_word( reinterpret_cast<const uint8_t*>( data ) );
    }
    uint32_t to_dword( const char* data )
    {
        return to_dword( reinterpret_cast<const uint8_t*>( data ) );
    }
    uint64_t to_qword( const char* data )
    {
        return to_qword( reinterpret_cast<const uint8_t*>( data ) );
    }

    uint16_t to_word( const uint8_t* data )
    {
        assert( data );
        if ( m_big_endian )
        {
            return  ( static_cast<uint16_t>( data[0] ) << 8 ) |
                      static_cast<uint16_t>( data[1] );
        }
        return ( static_cast<uint16_t>( data[1] ) << 8 ) |
                 static_cast<uint16_t>( data[0] );
    }

    uint32_t to_dword( const uint8_t* data )
    {
        assert( data );
        if ( m_big_endian )
        {
            return  ( static_cast<uint32_t>( data[0] ) << 24 ) |
                    ( static_cast<uint32_t>( data[1] ) << 16 ) |
                    ( static_cast<uint32_t>( data[2] ) << 8 ) |
                      static_cast<uint32_t>( data[3] );
        }
        return  ( static_cast<uint32_t>( data[3] ) << 24 ) |
                ( static_cast<uint32_t>( data[2] ) << 16 ) |
                ( static_cast<uint32_t>( data[1] ) << 8 ) |
                  static_cast<uint32_t>( data[0] );
    }

    uint64_t to_qword( const uint8_t* data )
    {
        assert( data );
        if ( m_big_endian )
        {
            return  ( static_cast<uint64_t>( data[0] ) << 56 ) |
                    ( static_cast<uint64_t>( data[1] ) << 48 ) |
                    ( static_cast<uint64_t>( data[2] ) << 40 ) |
                    ( static_cast<uint64_t>( data[3] ) << 32 ) |
                    ( static_cast<uint64_t>( data[4] ) << 24 ) |
                    ( static_cast<uint64_t>( data[5] ) << 16 ) |
                    ( static_cast<uint64_t>( data[6] ) << 8 ) |
                      static_cast<uint64_t>( data[7] );
        }
        return  ( static_cast<uint64_t>( data[7] ) << 56 ) |
                ( static_cast<uint64_t>( data[6] ) << 48 ) |
                ( static_cast<uint64_t>( data[5] ) << 40 ) |
                ( static_cast<uint64_t>( data[4] ) << 32 ) |
                ( static_cast<uint64_t>( data[3] ) << 24 ) |
                ( static_cast<uint64_t>( data[2] ) << 16 ) |
                ( static_cast<uint64_t>( data[1] ) << 8 ) |
                  static_cast<uint64_t>( data[0] );
    }

    void from_word( uint16_t x, char* buf )
    {
        from_word( x, reinterpret_cast<uint8_t*>( buf ) );
    }
    void from_dword( uint32_t x, char* buf )
    {
        from_dword( x, reinterpret_cast<uint8_t*>( buf ) );
    }
    void from_qword( uint64_t x, char* buf )
    {
        from_qword( x, reinterpret_cast<uint8_t*>( buf ) );
    }

    void from_word( uint16_t x, uint8_t* buf )
    {
        assert(buf);
        if ( m_big_endian )
        {
            buf[0] = high_byte( x );
            buf[1] = low_byte( x );
        }
        else
        {
            buf[1] = high_byte( x );
            buf[0] = low_byte( x );
        }
    }
    void from_dword( uint32_t x, uint8_t* buf )
    {
        assert(buf);
        if ( m_big_endian )
        {
            buf[0] = static_cast<uint8_t>( ( x & 0xFF000000UL ) >> 24 );
            buf[1] = static_cast<uint8_t>( ( x & 0xFF0000UL ) >> 16 );
            buf[2] = static_cast<uint8_t>( ( x & 0xFF00UL ) >> 8 );
            buf[3] = static_cast<uint8_t>( x & 0xFFUL );
        }
        else
        {
            buf[3] = static_cast<uint8_t>( ( x & 0xFF000000 ) >> 24 );
            buf[2] = static_cast<uint8_t>( ( x & 0xFF0000 ) >> 16 );
            buf[1] = static_cast<uint8_t>( ( x & 0xFF00 ) >> 8 );
            buf[0] = static_cast<uint8_t>( x & 0xFF );
        }
    }
    void from_qword( uint64_t x, uint8_t* buf )
    {
        assert(buf);
        uint32_t h = high_dword( x );
        uint32_t l = low_dword( x );
        if ( m_big_endian )
        {
            buf[0] = static_cast<uint8_t>( ( h & 0xFF000000UL ) >> 24 );
            buf[1] = static_cast<uint8_t>( ( h & 0xFF0000UL ) >> 16 );
            buf[2] = static_cast<uint8_t>( ( h & 0xFF00UL ) >> 8 );
            buf[3] = static_cast<uint8_t>( h & 0xFFUL );
            buf[4] = static_cast<uint8_t>( ( l & 0xFF000000UL ) >> 24 );
            buf[5] = static_cast<uint8_t>( ( l & 0xFF0000UL ) >> 16 );
            buf[6] = static_cast<uint8_t>( ( l & 0xFF00UL ) >> 8 );
            buf[7] = static_cast<uint8_t>( l & 0xFFUL );
        }
        else
        {
            buf[7] = static_cast<uint8_t>( ( h & 0xFF000000UL ) >> 24 );
            buf[6] = static_cast<uint8_t>( ( h & 0xFF0000UL ) >> 16 );
            buf[5] = static_cast<uint8_t>( ( h & 0xFF00UL ) >> 8 );
            buf[4] = static_cast<uint8_t>( h & 0xFFUL );
            buf[3] = static_cast<uint8_t>( ( l & 0xFF000000UL ) >> 24 );
            buf[2] = static_cast<uint8_t>( ( l & 0xFF0000UL ) >> 16 );
            buf[1] = static_cast<uint8_t>( ( l & 0xFF00UL ) >> 8 );
            buf[0] = static_cast<uint8_t>( l & 0xFFUL );
        }
    }

private:
    bool m_big_endian;
};

}

#ifdef XUL_RUN_TEST

#include <xul/util/test_case.hpp>

class bit_converter_test_case : public xul::test_case
{
protected:
    virtual void do_run()
    {
        using xul::bit_converter;
        {
            bit_converter bc( true );
            assert( bc.to_word("\x12\x33") != 0x1234 );
            assert( bc.to_word("\x12\x34") == 0x1234 );
            assert( bc.to_dword("\x12\x34\x56\x18") != 0x12345678 );
            assert( bc.to_dword("\x12\x34\x56\x78") == 0x12345678 );

            uint8_t buf[4];
            bc.from_word( 0x1234, buf );
            assert( memcmp("\x12\x34", buf, 2 ) == 0 );
            bc.from_dword( 0x12345678, buf );
            assert( memcmp("\x12\x34\x56\x78", buf, 4 ) == 0 );
        }
        {
            bit_converter bc( false );
            assert( bc.to_word("\x12\x31") != 0x3412 );
            assert( bc.to_word("\x12\x34") == 0x3412 );
            assert( bc.to_dword("\x12\x34\x56\x71") != 0x78563412 );
            assert( bc.to_dword("\x12\x34\x56\x78") == 0x78563412 );

            uint8_t buf[4];
            bc.from_word( 0x3412, buf );
            assert( memcmp("\x12\x34", buf, 2 ) == 0 );
            bc.from_dword( 0x78563412, buf );
            assert( memcmp("\x12\x34\x56\x78", buf, 4 ) == 0 );
        }
        assert(bit_converter::make_word(0x34, 0x12) == 0x3412);
        assert(bit_converter::low_byte(0x3412) == 0x12);
        assert(bit_converter::high_byte(0x3412) == 0x34);
        assert(bit_converter::make_dword(0x5678, 0x1234) == 0x56781234UL);
        assert(bit_converter::low_word(0x56781234UL) == 0x1234);
        assert(bit_converter::high_word(0x56781234UL) == 0x5678);

#if defined(_MSC_VER) && _MSC_VER < 1300
        assert(bit_converter::make_qword(0x90abcdefUL, 0x12345678UL) == 0x90abcdef12345678ULL);
        assert(bit_converter::low_dword(0x90abcdef12345678ULL) == 0x12345678UL);
        assert(bit_converter::high_dword(0x90abcdef12345678ULL) == 0x90abcdefUL);
        {
            bit_converter bc( true );
            assert( bc.to_qword("\x12\x34\x56\x78\x90\xab\xcd\xef") == 0x1234567890abcdefULL );
            uint8_t buf[8];
            bc.from_qword( 0x1234567890abcdefULL, buf );
            assert( memcmp("\x12\x34\x56\x78\x90\xab\xcd\xef", buf, 8 ) == 0 );
        }
        {
            bit_converter bc( false );
            assert( bc.to_qword("\x12\x34\x56\x78\x90\xab\xcd\xef") == 0xefcdab9078563412ULL );
            uint8_t buf[8];
            bc.from_qword( 0xefcdab9078563412ULL, buf );
            assert( memcmp("\x12\x34\x56\x78\x90\xab\xcd\xef", buf, 8 ) == 0 );
        }
#else
        assert(bit_converter::make_qword(0x90abcdefUL, 0x12345678UL) == 0x90abcdef12345678ULL);
        assert(bit_converter::low_dword(0x90abcdef12345678ULL) == 0x12345678UL);
        assert(bit_converter::high_dword(0x90abcdef12345678ULL) == 0x90abcdefUL);
        {
            bit_converter bc( true );
            assert( bc.to_qword("\x12\x34\x56\x78\x90\xab\xcd\xef") == 0x1234567890abcdefULL );
            uint8_t buf[8];
            bc.from_qword( 0x1234567890abcdefULL, buf );
            assert( memcmp("\x12\x34\x56\x78\x90\xab\xcd\xef", buf, 8 ) == 0 );
        }
        {
            bit_converter bc( false );
            assert( bc.to_qword("\x12\x34\x56\x78\x90\xab\xcd\xef") == 0xefcdab9078563412ULL );
            uint8_t buf[8];
            bc.from_qword( 0xefcdab9078563412ULL, buf );
            assert( memcmp("\x12\x34\x56\x78\x90\xab\xcd\xef", buf, 8 ) == 0 );
        }
#endif
    }
};

//XUL_TEST_SUITE_REGISTRATION(bit_converter_test_case);


#endif




