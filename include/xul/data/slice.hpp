#pragma once

#include <string>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>


namespace xul {


class slice
{
public:
    slice() : m_data(""), m_size(0) { }
    slice(const uint8_t* d, size_t n) : m_data(reinterpret_cast<const char*>(d)), m_size(n) { }
    slice(const char* d, size_t n) : m_data(d), m_size(n) { }
    explicit slice(const std::string& s) : m_data(s.data()), m_size(s.size()) { }
    explicit slice(const char* s) : m_data(s), m_size(strlen(s)) { }

    const uint8_t* data() const { return reinterpret_cast<const uint8_t*>(m_data); }

    size_t size() const { return m_size; }

    bool empty() const { return m_size == 0; }

    char operator[](size_t n) const
    {
        assert(n < size());
        return m_data[n];
    }

    void clear()
    {
        m_data = "";
        m_size = 0;
    }

    void remove_prefix(size_t n)
    {
        assert(n <= size());
        m_data += n;
        m_size -= n;
    }

    std::string str() const
    {
        return std::string(m_data, m_size);
    }

    int compare(const slice& b) const
    {
        const size_t min_len = (m_size < b.m_size) ? m_size : b.m_size;
        int r = memcmp(m_data, b.m_data, min_len);
        if (r == 0)
        {
            if (m_size < b.m_size)
                r = -1;
            else if (m_size > b.m_size)
                r = +1;
        }
        return r;
    }

    bool starts_with(const slice& x) const
    {
        return ((m_size >= x.m_size) && (memcmp(m_data, x.m_data, x.m_size) == 0));
    }
    
private:
    const char* m_data;
    size_t m_size;
};


inline bool operator==(const slice& x, const slice& y) {
    return ((x.size() == y.size()) &&
            (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const slice& x, const slice& y) {
    return !(x == y);
}


}
