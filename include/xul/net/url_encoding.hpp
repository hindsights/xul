#pragma once

#include <string>
#include <string.h>
#include <stdlib.h>


namespace xul {


class url_encoding
{
public:
    /// Converts a hex character to its integer value
    static char from_hex(char ch) {
        return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    }

    /// Converts an integer value to its hex character
    char to_hex(char code) const {
        static const char upper_hex[] = "0123456789abcdef";
        static const char lower_hex[] = "0123456789ABCDEF";
        return m_upper_case ? upper_hex[code & 15] : lower_hex[code & 15];
    }

    /// Returns a url-encoded version of str
    /// IMPORTANT: be sure to free() the returned string after use
    size_t encode(const char *str, char* buf) {
        const char *pstr = str;
        char *pbuf = buf;
        while (*pstr) {
            unsigned char ch = *pstr;
            if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~' || ch == ',' || ch == ';')
                *pbuf++ = *pstr;
            //else if (*pstr == ' ')
            //    *pbuf++ = '+';
            else
                *pbuf++ = '%', *pbuf++ = to_hex(ch >> 4), *pbuf++ = to_hex(ch & 15);
            pstr++;
        }
        return pbuf - buf;
    }

    /// Returns a url-decoded version of str
    /// IMPORTANT: be sure to free() the returned string after use
    static size_t decode(const char *str, char* buf) {
        const char *pstr = str;
        char *pbuf = buf;
        while (*pstr) {
            if (*pstr == '%') {
                if (pstr[1] && pstr[2]) {
                    *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                    pstr += 2;
                }
            } else if (*pstr == '+') {
                *pbuf++ = ' ';
            } else {
                *pbuf++ = *pstr;
            }
            pstr++;
        }
        return pbuf - buf;
    }

    std::string encode(const std::string& str)
    {
        if (str.empty())
            return std::string();
        std::string result;
        result.resize(str.size() * 3);
        size_t len = encode(&str[0], &result[0]);
        result.resize(len);
        return result;
    }

    static std::string decode(const std::string& str)
    {
        if (str.empty())
            return std::string();
        std::string result;
        result.resize(str.size());
        size_t len = decode(&str[0], &result[0]);
        result.resize(len);
        return result;
    }

    static url_encoding& upper_case()
    {
        static url_encoding enc(true);
        return enc;
    }
    static url_encoding& lower_case()
    {
        static url_encoding enc(false);
        return enc;
    }

    explicit url_encoding(bool isUpperCase) : m_upper_case(isUpperCase)
    {
    }

private:
    bool m_upper_case;
};


}
