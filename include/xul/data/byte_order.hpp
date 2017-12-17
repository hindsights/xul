#pragma once

#include <xul/data/bit_converter.hpp>

#ifdef _WIN32_WCE
#define BOOST_LITTLE_ENDIAN
#else
#include <boost/detail/endian.hpp>
#endif

#include <assert.h>
#include <stdint.h>

#if defined(BOOST_BIG_ENDIAN)
const bool xul_host_byte_order_is_big_endian = true;
#elif defined(BOOST_LITTLE_ENDIAN)
const bool xul_host_byte_order_is_big_endian = false;
#else
#error invalid platform for boost endian!!!!!!
#endif


namespace xul {


class byte_order
{
public:
    static uint16_t reverse_word( uint16_t val )
    {
        //UINT16 result;
        //BYTE* dest = reinterpret_cast<BYTE*>( &result );
        //const BYTE* src = reinterpret_cast<const BYTE*>( &val );
        //dest[0] = src[1];
        //dest[1] = src[0];
        //return result;
        return ((val & 0xff) << 8) | ((val & 0xff00) >> 8);
    }

    static uint32_t reverse_dword( uint32_t n )
    {
        //UINT32 result;
        //BYTE* dest = reinterpret_cast<BYTE*>( &result );
        //const BYTE* src = reinterpret_cast<const BYTE*>( &val );
        //dest[0] = src[3];
        //dest[1] = src[2];
        //dest[2] = src[1];
        //dest[3] = src[0];
        //return result;
        return ((n & 0xff) << 24)
             | ((n & 0xff00) << 8)
             | ((n & 0xff0000UL) >> 8)
             | ((n & 0xff000000UL) >> 24);
    }

    static uint64_t reverse_qword( uint64_t x )
    {
        //UINT64 result;
        //BYTE* dest = reinterpret_cast<BYTE*>( &result );
        //const BYTE* src = reinterpret_cast<const BYTE*>( &val );
        //dest[0] = src[7];
        //dest[1] = src[6];
        //dest[2] = src[5];
        //dest[3] = src[4];
        //dest[4] = src[3];
        //dest[5] = src[2];
        //dest[6] = src[1];
        //dest[7] = src[0];
        //return result;
        uint32_t low = reverse_dword( bit_converter::high_dword( x ) );
        uint32_t high = reverse_dword( bit_converter::low_dword( x ) );
        return bit_converter::make_qword( high, low );
    }


    byte_order( bool isBigEndian ) : m_big_endian( isBigEndian )
    {
        static host_byte_order_checker _checker;
    }

    void set_big_endian( bool isBigEndian )
    {
        m_big_endian = isBigEndian;
    }
    bool is_big_endian() const
    {
        return m_big_endian;
    }

    uint16_t convert_word( uint16_t val )
    {
        if ( xul_host_byte_order_is_big_endian == m_big_endian )
            return val;
        return byte_order::reverse_word( val );
    }

    uint32_t convert_dword( uint32_t val )
    {
        if ( xul_host_byte_order_is_big_endian == m_big_endian )
            return val;
        return byte_order::reverse_dword( val );
    }

    uint64_t convert_qword( uint64_t val )
    {
        if ( xul_host_byte_order_is_big_endian == m_big_endian )
            return val;
        return byte_order::reverse_qword( val );
    }

    double convert_double( double val )
    {
        if ( xul_host_byte_order_is_big_endian == m_big_endian )
            return val;
        const char* src = (const char*)&val;
        double res;
        char* dst = (char*)&res;
        std::reverse_copy(src, src + sizeof(double), dst);
        return res;
    }

private:
    class host_byte_order_checker
    {
    public:
        host_byte_order_checker()
        {
            assert( xul_host_byte_order_is_big_endian == big_endian() );
        }
        static bool big_endian()
        {
            short tester = 0x0201;
            return  *(char*)&tester==2;
        }
    };

    bool m_big_endian;
};

}

#ifdef XUL_RUN_TEST

#include <xul/util/test_case.hpp>

class byte_order_test_case : public xul::test_case
{
protected:
    typedef xul::byte_order byte_order;

    virtual void do_run()
    {
        test_reverse();
        test_convert();
    }
    void test_reverse()
    {
        assert(byte_order::reverse_word(0x1234) == 0x3412);
        assert(byte_order::reverse_dword(0x12345678UL) == 0x78563412UL);
#if defined(_MSC_VER) && _MSC_VER < 1300
        assert(byte_order::reverse_qword(0x1234567890abcdefUI64) == 0xefcdab9078563412UI64);
#else
        assert(byte_order::reverse_qword(0x1234567890abcdefULL) == 0xefcdab9078563412ULL);
#endif
    }

    void test_convert()
    {
        if ( xul_host_byte_order_is_big_endian )
        {
            {
                byte_order hbc(true);
                assert(hbc.convert_word(0x1234) == 0x1234);
                assert(hbc.convert_dword(0x12345678UL) == 0x12345678UL);
#if defined(_MSC_VER) && _MSC_VER < 1300
                assert(hbc.convert_qword(0x1234567890abcdefUI64) == 0x1234567890abcdefUI64);
#else
                assert(hbc.convert_qword(0x1234567890abcdefULL) == 0x1234567890abcdefULL);
#endif
            }
            {
                byte_order hbc(false);
                assert(hbc.convert_word(0x1234) == 0x3412);
                assert(hbc.convert_dword(0x12345678UL) == 0x78563412UL);
#if defined(_MSC_VER) && _MSC_VER < 1300
                assert(hbc.convert_qword(0x1234567890abcdefUI64) == 0xefcdab9078563412UI64);
#else
                assert(hbc.convert_qword(0x1234567890abcdefULL) == 0xefcdab9078563412ULL);
#endif
            }
        }
        else
        {
            {
                byte_order hbc(false);
                assert(hbc.convert_word(0x1234) == 0x1234);
                assert(hbc.convert_dword(0x12345678UL) == 0x12345678UL);
#if defined(_MSC_VER) && _MSC_VER < 1300
                assert(hbc.convert_qword(0x1234567890abcdefUI64) == 0x1234567890abcdefUI64);
#else
                assert(hbc.convert_qword(0x1234567890abcdefULL) == 0x1234567890abcdefULL);
#endif
            }
            {
                byte_order hbc(true);
                assert(hbc.convert_word(0x1234) == 0x3412);
                assert(hbc.convert_dword(0x12345678UL) == 0x78563412UL);
#if defined(_MSC_VER) && _MSC_VER < 1300
                assert(hbc.convert_qword(0x1234567890abcdefUI64) == 0xefcdab9078563412UI64);
#else
                assert(hbc.convert_qword(0x1234567890abcdefULL) == 0xefcdab9078563412ULL);
#endif
            }
        }
    }
};

//XUL_TEST_SUITE_REGISTRATION(byte_order_test_case);

#endif


