#pragma once

#include <xul/text/hex_encoding.hpp>

#include <string>
#include <ostream>
#include <stdint.h>
#include <string.h>


namespace xul {


template <int LengthT>
class big_number
{
public:
    // the number of bytes of the number
    enum { byte_size = LengthT };

    static int size()
    {
        return byte_size * 2;
    }

    big_number() { clear(); }

    explicit big_number(const uint8_t* data, size_t size)
    {
    }

    explicit big_number(std::string const& s)
    {
    }

    void assign(std::string const& s)
    {
        assert(s.size() >= 20);
        int sl = int(s.size()) < size() ? int(s.size()) : size();
        memcpy(m_number, &s[0], sl);
    }

    void assign(const char* str) { memcpy(m_number, str, byte_size); }
    void assign(const uint8_t* str) { memcpy(m_number, str, byte_size); }
    void clear() { memset(m_number, 0, byte_size); }

    bool is_null() const
    {
        for (const unsigned char* i = m_number; i < m_number+byte_size; ++i)
            if (*i != 0) return false;
        return true;
    }
    bool is_zero() const
    {
        return is_null();
    }

    bool operator==(big_number const& n) const
    {
        return std::equal(n.m_number, n.m_number+byte_size, m_number);
    }

    bool operator!=(big_number const& n) const
    {
        return !std::equal(n.m_number, n.m_number+byte_size, m_number);
    }

    bool operator<(big_number const& n) const
    {
        for (int i = 0; i < byte_size; ++i)
        {
            if (m_number[i] < n.m_number[i]) return true;
            if (m_number[i] > n.m_number[i]) return false;
        }
        return false;
    }

    big_number operator~()
    {
        big_number ret;
        for (int i = 0; i< byte_size; ++i)
            ret.m_number[i] = ~m_number[i];
        return ret;
    }

    big_number& operator &= (big_number const& n)
    {
        for (int i = 0; i< byte_size; ++i)
            m_number[i] &= n.m_number[i];
        return *this;
    }

    big_number& operator |= (big_number const& n)
    {
        for (int i = 0; i< byte_size; ++i)
            m_number[i] |= n.m_number[i];
        return *this;
    }

    big_number& operator ^= (big_number const& n)
    {
        for (int i = 0; i< byte_size; ++i)
            m_number[i] ^= n.m_number[i];
        return *this;
    }

    unsigned char& operator[](int i)
    { assert(i >= 0 && i < byte_size); return m_number[i]; }

    unsigned char const& operator[](int i) const
    { assert(i >= 0 && i < byte_size); return m_number[i]; }

    typedef const unsigned char* const_iterator;
    typedef unsigned char* iterator;

    const_iterator begin() const { return m_number; }
    const_iterator end() const { return m_number+byte_size; }

    iterator begin() { return m_number; }
    iterator end() { return m_number+byte_size; }

    std::string to_string() const
    { return std::string((char const*)&m_number[0], byte_size); }

    std::string str() const
    {
        return xul::hex_encoding::lower_case().encode(m_number, byte_size);
    }
    std::string bytes() const
    { return std::string((char const*)&m_number[0], byte_size); }
    const unsigned char* data() const { return m_number; }
    //size_t size() const { return byte_size; }

    bool decode(const std::string& s)
    {
        if (s.size() != byte_size * 2)
            return false;
        return xul::hex_encoding::decode(s, m_number, byte_size);
    }

private:
    unsigned char m_number[byte_size];
};


template <int LengthT>
inline std::ostream& operator<<(std::ostream& os, big_number<LengthT> const& num)
{
    return os << num.str();
}


}
