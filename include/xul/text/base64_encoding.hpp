#pragma once

#include <boost/static_assert.hpp>

#include <string>

#include <assert.h>

namespace xul {


class base64_encoding
{
public:
    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> encode(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        string_type result;
        size_t size = calc_enough_encoded_length(str.size());
        if (size > 0)
        {
            result.resize(size);
            size_t len = do_encode(str.data(), str.size(), &result[0]);
            result.resize(len);
        }
        return result;
    }

    static std::string encode( const char* data, size_t size )
    {
        std::string result;
        size_t maybeResultSize = calc_enough_encoded_length(size);
        if (maybeResultSize > 0)
        {
            result.resize(maybeResultSize);
            size_t len = do_encode(data, size, &result[0]);
            result.resize(len);
        }
        return result;
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> decode(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        string_type result;
        size_t size = calc_enough_decoded_length(str.size());
        if (size > 0)
        {
            result.resize(size);
            size_t len = do_decode(str.data(), str.size(), &result[0]);
            result.resize(len);
        }
        return result;
    }


    static std::string& get_delimiter()
    {
        //static std::string delimiter = "\r\n";
        static std::string delimiter = "";
        return delimiter;
    }
    static const char* get_encoding_table()
    {
        static const char EnBase64Table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        BOOST_STATIC_ASSERT(sizeof(EnBase64Table) == 64 + 1);
        return EnBase64Table;
    }
    static const char* get_url_safe_encoding_table()
    {
        static const char url_safe_encoding_table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        BOOST_STATIC_ASSERT(sizeof(url_safe_encoding_table) == 64 + 1);
        return url_safe_encoding_table;
    }
    static const char* get_decoding_table()
    {
        static const char DeBase64Table[256] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            62,            // '+'
            0,            // ','
            62,            // '-'
            0,
            63,        // '/'
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
            0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
            0, 0, 0,
            0,            // '^'
            63,            // '_'
            0,            // '`'
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
        };
        BOOST_STATIC_ASSERT(sizeof(DeBase64Table) == 256);
        return DeBase64Table;
    }
    static const char* get_url_safe_decoding_table()
    {
        static const char url_safe_decoding_table[256] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0,            // '+'
            0,            // ','
            62,            // '-'
            0,
            0,        // '/'
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
            0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
            0, 0, 0,
            0,            // '^'
            63,            // '_'
            0,            // '`'
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
        };
        BOOST_STATIC_ASSERT(sizeof(url_safe_decoding_table) == 256);
        return url_safe_decoding_table;
    }


public:
    enum { _MAX_LINE_LENGTH = 72 };

    template <typename InputIteratorT, typename OutputIteratorT>
    static size_t do_encode(InputIteratorT pSrc, size_t nSrcLen, OutputIteratorT pDst)
    {
        const size_t nMaxLineLen = _MAX_LINE_LENGTH;
        unsigned char c1, c2, c3;
        size_t nDstLen = 0;
        size_t nLineLen = 0;
        size_t nDiv = nSrcLen / 3;
        size_t nMod = nSrcLen % 3;
        const char* EnBase64Tab = get_encoding_table();
        for (size_t i = 0; i < nDiv; i ++)
        {
            c1 = *pSrc++;
            c2 = *pSrc++;
            c3 = *pSrc++;
            *pDst++ = EnBase64Tab[c1 >> 2];
            *pDst++ = EnBase64Tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
            *pDst++ = EnBase64Tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
            *pDst++ = EnBase64Tab[c3 & 0x3f];
            nLineLen += 4;
            nDstLen += 4;
            if (nLineLen > nMaxLineLen - 4)
            {
                const std::string& delimiter = get_delimiter();
                if (delimiter.size() > 0)
                {
                    std::copy(delimiter.begin(), delimiter.end(), pDst);
                    nDstLen += delimiter.size();
                }
                nLineLen = 0;
            }
        }
        if (nMod == 1)
        {
            c1 = *pSrc++;
            *pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
            *pDst++ = EnBase64Tab[((c1 & 0x03) << 4)];
            *pDst++ = '=';
            *pDst++ = '=';
            nLineLen += 4;
            nDstLen += 4;
        }
        else if (nMod == 2)
        {
            c1 = *pSrc++;
            c2 = *pSrc++;
            *pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
            *pDst++ = EnBase64Tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
            *pDst++ = EnBase64Tab[((c2 & 0x0f) << 2)];
            *pDst++ = '=';
            nDstLen += 4;
        }
        return nDstLen;
    }

    template <typename InputIteratorT, typename OutputIteratorT>
    static size_t do_decode(InputIteratorT pSrc, size_t nSrcLen, OutputIteratorT pDst)
    {
        size_t nValue;
        size_t nDstLen = 0;
        size_t i = 0;
        const char* DeBase64Tab = get_decoding_table();
        while (i < nSrcLen)
        {
            if (*pSrc != '\r' && *pSrc != '\n')
            {
                nValue = DeBase64Tab[*pSrc++] << 18;
                nValue += DeBase64Tab[*pSrc++] << 12;
                *pDst++ = static_cast<char>( (nValue & 0x00ff0000) >> 16 );
                nDstLen++;
                if (*pSrc != '=')
                {
                    nValue += DeBase64Tab[*pSrc++] << 6;
                    *pDst++ = static_cast<char>( (nValue & 0x0000ff00) >> 8 );
                    nDstLen++;
                    if (*pSrc != '=')
                    {
                        nValue += DeBase64Tab[*pSrc++];
                        *pDst++ = static_cast<char>( nValue & 0x000000ff );
                        nDstLen++;
                    }
                }
                i += 4;
            }
            else
            {
                pSrc++;
                i++;
            }
        }
        return nDstLen;
    }

private:
    static size_t calc_enough_encoded_length(size_t length)
    {
        size_t len = length / 3;
        if (length % 3 > 0)
            ++len;
        len *= 4;
        size_t line = len / _MAX_LINE_LENGTH;
        if (len % _MAX_LINE_LENGTH > 0)
            ++line;
        len += line * 2;
        return len;
    }

    static size_t calc_enough_decoded_length(size_t length)
    {
        return length;
    }

private:
    static unsigned char get_first_six_bits(unsigned char val)
    {
        return (val & 0xFC) >> 2;
    }
    static unsigned char get_last_six_bits(unsigned char val)
    {
        return val & 0x3F;
    }
};


}



