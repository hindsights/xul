#pragma once

#include <string>


namespace xul {


template <typename CharT, typename TraitsT, typename AllocT>
class basic_string_slice
{
public:
    typedef std::basic_string<CharT, TraitsT, AllocT> string_type;
    typedef typename string_type::size_type size_type;

    basic_string_slice(const string_type& s, size_type start = 0, size_type end = string_type::npos)
        : m_string(s)
        , m_start(start)
        , m_end(end)
    {
    }

    const string_type& source() const { return m_string; }
    size_type start() const { return m_start; }
    size_type end() const { return m_end; }

private:
    const string_type& m_string;
    const size_type m_start;
    const size_type m_end;
};


}
