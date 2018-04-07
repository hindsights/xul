#pragma once

#include <xul/text/hex_encoding.hpp>
#include <xul/std/std_hasher.hpp>

#include <string>
#include <stdint.h>
#include <stddef.h>
#include <string.h>


namespace xul {


/// big number utility class (for little-endian platform only)
template <size_t LengthT>
class big_number
{
public:
    // the number of bytes of the number
    enum { byte_size = LengthT };
    static const size_t byte_count = LengthT;
    static const size_t bit_count = LengthT * 8;

    static size_t size()
    {
        return byte_count;
    }

    big_number() { this->clear(); }

    void assign(uint32_t val)
    {
        this->clear();
        memcpy(this->m_bytes, &val, 4);
    }

    void assign(const uint64_t& val)
    {
        this->clear();
        memcpy(this->m_bytes, &val, 8);
    }

    void assign(std::string const& s)
    {
        assert(s.size() >= 20);
        int sl = int(s.size()) < size() ? int(s.size()) : size();
        memcpy(this->m_bytes, &s[0], sl);
    }

    void assign(const char* str) { memcpy(this->m_bytes, str, byte_count); }
    void assign(const uint8_t* str) { memcpy(this->m_bytes, str, byte_count); }
    void clear() { memset(this->m_bytes, 0, byte_count); }

    bool is_null() const
    {
        for (const unsigned char* i = m_bytes; i < this->m_bytes+byte_count; ++i)
            if (*i != 0) return false;
        return true;
    }
    bool is_zero() const
    {
        return is_null();
    }

    bool operator==(big_number const& n) const
    {
        return this->compare(n) == 0;
    }

    bool operator!=(big_number const& n) const
    {
        return this->compare(n) != 0;
    }

    bool operator<(big_number const& n) const
    {
        return this->compare(n) < 0;
    }
    bool operator>(big_number const& n) const
    {
        return this->compare(n) > 0;
    }
    bool operator<=(big_number const& n) const
    {
        return this->compare(n) <= 0;
    }
    bool operator>=(big_number const& n) const
    {
        return this->compare(n) >= 0;
    }
    int compare(big_number const& n) const
    {
        for (int i = byte_count - 1; i >= 0; --i)
        {
            if (this->m_bytes[i] < n.m_bytes[i]) return -1;
            if (this->m_bytes[i] > n.m_bytes[i]) return 1;
        }
        return 0;
    }

    big_number operator~() const
    {
        big_number ret;
        for (int i = 0; i< byte_count; ++i)
            ret.m_bytes[i] = ~this->m_bytes[i];
        return ret;
    }

    big_number& operator &= (big_number const& n)
    {
        for (int i = 0; i< byte_count; ++i)
            this->m_bytes[i] &= n.m_bytes[i];
        return *this;
    }

    big_number& operator |= (big_number const& n)
    {
        for (int i = 0; i< byte_count; ++i)
            this->m_bytes[i] |= n.m_bytes[i];
        return *this;
    }

    big_number& operator ^= (big_number const& n)
    {
        for (int i = 0; i< byte_count; ++i)
            this->m_bytes[i] ^= n.m_bytes[i];
        return *this;
    }

    big_number operator<<(unsigned int shift) const
    {
        big_number ret = *this;
        ret <<= shift;
        return ret;
    }

    big_number operator>>(unsigned int shift) const
    {
        big_number ret = *this;
        ret >>= shift;
        return ret;
    }

    // for little-endian sysstem only
    big_number& operator<<=(unsigned int shift)
    {
        if (shift == 0)
            return *this;
        if (shift >= bit_count)
        {
            this->clear();
            return *this;
        }
        int byteShift = shift / 8;
        int bitShift = shift % 8;
        assert(byteShift < byte_count);
        if (bitShift == 0)
        {
            assert(byteShift > 0);
            // only shifting bytes
            for (int i = static_cast<int>(byte_count) - byteShift - 1; i >= 0; --i)
            {
                this->m_bytes[i + byteShift] = this->m_bytes[i];
            }
        }
        else
        {
            int highBitShift = 8 - bitShift;
            for (int i = static_cast<int>(byte_count) - byteShift - 1; i > 0; --i)
            {
                this->m_bytes[i + byteShift] = (this->m_bytes[i] << bitShift) | (this->m_bytes[i-1] >> highBitShift);
            }
            this->m_bytes[byteShift] = this->m_bytes[0] << bitShift;
        }
        for (int i = 0; i < byteShift; ++i)
        {
            this->m_bytes[i] = 0;
        }
        return *this;
    }

    // for little-endian sysstem only
    big_number& operator>>=(unsigned int shift)
    {
        if (shift == 0)
            return *this;
        if (shift >= bit_count)
        {
            this->clear();
            return *this;
        }
        int byteShift = shift / 8;
        int bitShift = shift % 8;
        assert(byteShift < byte_count);
        if (bitShift == 0)
        {
            assert(byteShift > 0);
            // only shifting bytes
            for (int i = 0; i< byte_count - byteShift; ++i)
            {
                this->m_bytes[i] = this->m_bytes[i + byteShift];
            }
        }
        else
        {
            int highBitShift = 8 - bitShift;
            int rightMostByte = static_cast<int>(byte_count) - byteShift - 1;
            for (int i = 0; i < rightMostByte; ++i)
            {
                this->m_bytes[i] = (this->m_bytes[i + byteShift] >> bitShift) | (this->m_bytes[i + byteShift + 1] << highBitShift);
            }
            this->m_bytes[rightMostByte] = this->m_bytes[byte_count - 1] >> bitShift;
        }
        for (int i = byte_count - byteShift; i < byte_count; ++i)
        {
            this->m_bytes[i] = 0;
        }
        return *this;
    }

    unsigned char& operator[](int i)
    { assert(i >= 0 && i < byte_count); return this->m_bytes[i]; }

    unsigned char const& operator[](int i) const
    { assert(i >= 0 && i < byte_count); return this->m_bytes[i]; }

    typedef const unsigned char* const_iterator;
    typedef unsigned char* iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    const_iterator begin() const { return this->m_bytes; }
    const_iterator end() const { return this->m_bytes+byte_count; }

    iterator begin() { return this->m_bytes; }
    iterator end() { return this->m_bytes+byte_count; }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(this->end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(this->begin()); }

    reverse_iterator rbegin() { return reverse_iterator(this->m_bytes); }
    reverse_iterator rend() { return reverse_iterator(this->m_bytes+byte_count); }

    std::string format(bool useUppercase = true) const
    {
        return xul::hex_encoding(useUppercase).encode(this->rbegin(), this->rend());
    }
    std::string str() const { return this->format(false); }
    std::string bytes() const
    { return std::string((char const*)&this->m_bytes[0], byte_count); }
    const unsigned char* data() const { return this->m_bytes; }
//    size_t size() const { return byte_count; }

    bool try_parse(const std::string& s)
    {
        if (s.size() != byte_count * 2)
            return false;
        return xul::hex_encoding::decode(s, this->m_bytes, byte_count);
    }
    static big_number<LengthT> parse(const std::string& s)
    {
        big_number<LengthT> num;
        if (num.try_parse(s))
            return num;
        return big_number<LengthT>();
    }

private:
    unsigned char m_bytes[byte_count];
};


typedef big_number<16> uint128;
typedef big_number<20> uint160;
typedef big_number<32> uint256;
typedef big_number<64> uint512;


}


namespace std {

template<size_t LengthT> struct hash<xul::big_number<LengthT> >
{
public:
    size_t operator()(const xul::big_number<LengthT>& n) const
    {
        return xul::std_hasher::hash_bytes(n.data(), n.size());
    }
};

}
