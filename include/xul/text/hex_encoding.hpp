#pragma once

#include <boost/static_assert.hpp>

#include <string>
#include <string.h>
#include <assert.h>


namespace xul {



class hex_encoding
{
public:
    static const char* get_lower_hex_letters()
    {
        static const char* lowerLetters = "0123456789abcdef";
        return lowerLetters;
    }
    static const char* get_upper_hex_letters()
    {
        static const char* upperLetters = "0123456789ABCDEF";
        return upperLetters;
    }

    static hex_encoding& upper_case()
    {
        static hex_encoding enc(true);
        return enc;
    }
    static hex_encoding& lower_case()
    {
        static hex_encoding enc(false);
        return enc;
    }

    explicit hex_encoding(bool useUpperCase)
        : m_letters(useUpperCase ? get_upper_hex_letters() : get_lower_hex_letters())
    {
    }

    std::string encode(const unsigned char* data, size_t size)
    {
        if ((size == 0) || (data == NULL))
            return "";
        std::string result;
        result.resize(size * 2);
        do_encode(&result[0], data, data + size);
        return result;
    }
    std::string encode(const char* data, size_t size)
    {
        return encode(reinterpret_cast<const unsigned char*>(data), size);
    }
    std::string encode(const std::string& str)
    {
        return encode(str.data(), str.size());
    }

    static bool decode(const std::string& str, std::string& bytes)
    {
        bytes.clear();
        if (str.size() % 2 != 0)
            return false;
        bytes.reserve(str.size() / 2);
        std::string::size_type i = 0;
        while (i < str.size())
        {
            char ch1 = str[i++];
            char ch2 = str[i++];
            if (false == isxdigit(ch1) || false == isxdigit(ch2))
                return false;
            bytes.push_back(do_decode_byte(ch1, ch2));
        }
        return true;
    }
    static std::string decode(const std::string& str)
    {
        std::string s;
        if (decode(str, s))
            return s;
        return std::string();
    }
    static bool decode(const std::string& str, unsigned char* buf, size_t size)
    {
        if ( 0==str.size() || (str.size() % 2 != 0) )
            return false;
        if (size < str.size() / 2)
            return false;
        std::string::size_type i = 0;
        while (i < str.size())
        {
            char ch1 = str[i++];
            char ch2 = str[i++];
            if (false == isxdigit(ch1) || false == isxdigit(ch2))
                return false;
            *buf = do_decode_byte(ch1, ch2);
            ++buf;
        }
        return true;
    }

private:
    void do_encode(char* dest, const unsigned char* src, const unsigned char* srcEnd)
    {
        for (const unsigned char* pch = src; pch != srcEnd; ++pch)
        {
            unsigned char ch = *pch;
            unsigned char highBit = (ch >> 4);
            unsigned char lowBit = (ch & 0x0F);
            *dest++ = do_encode_bit(highBit);
            *dest++ = do_encode_bit(lowBit);
        }
    }

    char do_encode_bit(unsigned char bit)
    {
        assert(bit < 16);
        char ch = m_letters[bit];
        return ch;
    }

    static char do_decode_byte(char ch1, char ch2)
    {
        assert(isxdigit(ch1) && isxdigit(ch2));
        unsigned char highByte = decode_hex_byte(ch1);
        unsigned char lowByte = decode_hex_byte(ch2);
        assert(highByte < 16 && lowByte < 16);
        return (highByte << 4) | lowByte;
    }

    static unsigned char decode_hex_byte(char ch)
    {
        assert(isxdigit(ch));
        if (isdigit(ch))
        {
            return ch - '0';
        }
        if (isupper(ch))
            return ch - 'A' + 10;
        return ch - 'a' + 10;
    }

private:
    const char * const m_letters;
};


}



