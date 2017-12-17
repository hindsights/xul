#pragma once

#include <xul/std/tstring.hpp>
//#include <boost/lexical_cast.hpp>
#include <string>
#include <stdint.h>
#include <assert.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif


namespace xul {


template <typename T>
struct numeric_traits;


template <>
struct numeric_traits<short>
{
    typedef int value_type;

    static const char* ansi_format_string()
    {
        return "%hd";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%hd";
    }
};


template <>
struct numeric_traits<unsigned short>
{
    typedef unsigned int value_type;

    static const char* ansi_format_string()
    {
        return "%hu";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%hu";
    }
};


template <>
struct numeric_traits<int>
{
    typedef int value_type;

    static const char* ansi_format_string()
    {
        return "%d";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%d";
    }
};


template <>
struct numeric_traits<unsigned int>
{
    typedef unsigned int value_type;

    static const char* ansi_format_string()
    {
        return "%u";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%u";
    }
};


template <>
struct numeric_traits<long>
{
    typedef long value_type;

    static const char* ansi_format_string()
    {
        return "%ld";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%ld";
    }
};


template <>
struct numeric_traits<unsigned long>
{
    typedef unsigned long value_type;

    static const char* ansi_format_string()
    {
        return "%lu";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%lu";
    }
};



template <>
struct numeric_traits<long long>
{
    typedef long long value_type;

    static const char* ansi_format_string()
    {
        return "%lld";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%lld";
    }
};


template <>
struct numeric_traits<unsigned long long>
{
    typedef unsigned long long value_type;

    static const char* ansi_format_string()
    {
        return "%llu";
    }
    static const wchar_t* wide_format_string()
    {
        return L"%llu";
    }
};


template <>
struct numeric_traits<double>
{
    typedef double value_type;

    static const char* ansi_format_string()
    {
        return "%lf";
    }
    static const wchar_t* wide_format_string()
    {
        return L"lf";
    }
};


template <>
struct numeric_traits<float> : public numeric_traits<double>
{
    typedef float value_type;

    static const char* ansi_format_string()
    {
        return "%f";
    }
    static const wchar_t* wide_format_string()
    {
        return L"f";
    }
};


/// parsing and formating functionalities for numeric value
class numerics
{
public:
    template <typename ValueT>
    static ValueT parse(const std::string& s, ValueT defaultVal)
    {
        ValueT val = defaultVal;
        if (try_parse(s, val))
            return val;
        return defaultVal;
    }

    template <typename ValueT>
    static bool try_parse(const std::string& s, ValueT& val)
    {
        return 1 == sscanf(s.c_str(), numeric_traits<ValueT>::ansi_format_string(), &val);
    }

    template <typename ValueT>
    static std::string format(ValueT val)
    {
        char buf[64];
        buf[63] = '\0';
        int len = snprintf(buf, 63, numeric_traits<ValueT>::ansi_format_string(), val);
        return std::string(buf, len);
    }

    template <typename ValueT>
    static std::string format(ValueT val, const char* format_str)
    {
        char buf[64];
        buf[63] = '\0';
        int len = snprintf(buf, 63, format_str, val);
        return std::string(buf, len);
    }

    template <typename ValueT>
    static ValueT parse(const std::wstring& s, ValueT defaultVal)
    {
        ValueT val = defaultVal;
        if (try_parse(s, val))
            return val;
        return defaultVal;
    }
    template <typename ValueT>
    static bool try_parse(const std::wstring& s, ValueT& val)
    {
        return 1 == swscanf(s.c_str(), numeric_traits<ValueT>::wide_format_string(), &val);
    }


    template <typename ValueT>
    static std::wstring wformat(ValueT val)
    {
        wchar_t buf[64];
        buf[63] = L'\0';
        int len = swprintf(buf, numeric_traits<ValueT>::wide_format_string(), val);
        return std::wstring(buf, len);
    }


    template <typename ValueT>
    static ValueT get_min(ValueT x, ValueT y)
    {
        return (x < y) ? x : y;
    }

    template <typename ValueT>
    static ValueT get_max(ValueT x, ValueT y)
    {
        return (x > y) ? x : y;
    }

    template <typename ValueT>
    static ValueT calc_block_count(ValueT x, ValueT y)
    {
        assert(y > 0);
        return (x + y - 1) / y;
    }


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
};

}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
