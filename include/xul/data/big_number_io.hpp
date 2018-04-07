#pragma once

#include <xul/data/big_number.hpp>
#include <xul/io/data_input_stream.hpp>
#include <xul/io/data_output_stream.hpp>
#include <ostream>


namespace xul {

//#error reach big_number output
template <size_t LengthT>
inline data_input_stream& operator>>(data_input_stream& is, big_number<LengthT>& num)
{
    is.read_bytes(num.begin(), LengthT);
    return is;
}


template <size_t LengthT>
inline data_output_stream& operator<<(data_output_stream& os, const big_number<LengthT>& num)
{
    os.write_bytes(num.data(), LengthT);
    return os;
}

template <size_t LengthT>
inline std::ostream& operator<<(std::ostream& os, big_number<LengthT> const& num)
{
    return os << num.str();
}


}

