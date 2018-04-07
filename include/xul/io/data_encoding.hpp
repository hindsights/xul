#pragma once

#include <xul/io/data_input_stream.hpp>
#include <xul/io/data_output_stream.hpp>
#include <sstream>


namespace xul {


class data_encoding
{
public:
    static data_encoding big_endian()
    {
        return data_encoding(true);
    }
    static data_encoding little_endian()
    {
        return data_encoding(false);
    }

    explicit data_encoding(bool bigEndian) : m_big_endian(bigEndian) {}

    template <typename ...ARGS>
    std::string encode(const ARGS&... args)
    {
        std::ostringstream oss;
        ostream_data_output_stream os(oss, this->m_big_endian);
        this->do_encode(os, args...);
        return oss.str();
    }
    template <typename ...ARGS>
    bool decode(const std::string& s, ARGS&... args)
    {
        char dummybuf[1];
        memory_data_input_stream is(s.empty() ? dummybuf : s.data(), s.size(), this->m_big_endian);
        this->do_decode(is, args...);
        return is.good();
    }
    template <typename ...ARGS>
    bool decode(const uint8_t* data, int size, ARGS&... args)
    {
        memory_data_input_stream is(data, size, this->m_big_endian);
        this->do_decode(is, args...);
        return is.good();
    }

private:
    template <typename T>
    void do_encode(data_output_stream& os, const T& arg)
    {
        os << arg;
    }
    template <typename T, typename ...ARGS>
    void do_encode(data_output_stream& os, const T& arg, const ARGS&... rest)
    {
        os << arg;
        this->do_encode(os, rest...);
    }

    template <typename T>
    void do_decode(data_input_stream& is, T& arg)
    {
        is >> arg;
    }
    template <typename T, typename ...ARGS>
    void do_decode(data_input_stream& is, T& arg, ARGS&... rest)
    {
        is >> arg;
        this->do_decode(is, rest...);
    }
private:
    bool m_big_endian;
};


}
