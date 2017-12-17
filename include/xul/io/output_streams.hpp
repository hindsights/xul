#pragma once

#include <xul/io/output_stream.hpp>
#include <xul/data/printables.hpp>
#include <string>
#include <sstream>


namespace xul {


/// interface for output stream
class output_streams
{
public:
    static void write_string(output_stream& os, const std::string& s)
    {
        os.write_chars(s.c_str(), s.size());
    }
};


inline output_stream& operator<<(output_stream& os, uint8_t val)
{
    os.write_byte(val);
    return os;
}

inline output_stream& operator<<(output_stream& os, char val)
{
    os.write_char(val);
    return os;
}

inline output_stream& operator<<(output_stream& os, const char* s)
{
    os.write_string(s);
    return os;
}

inline output_stream& operator<<(output_stream& os, const std::string& s)
{
    output_streams::write_string(os, s);
    return os;
}

template <typename T>
inline output_stream& operator<<(output_stream& os, const T& v)
{
    std::ostringstream oss;
    oss << v;
    std::string s = oss.str();
    os << s;
    return os;
}


}
