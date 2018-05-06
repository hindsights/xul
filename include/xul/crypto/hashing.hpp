#pragma once

#include <xul/io/data_output_stream.hpp>
#include <xul/crypto/hash_stream.hpp>
#include <xul/text/hex_encoding.hpp>
//#include <xul/log/log.hpp>
#include <stdint.h>

namespace xul {


class hashing
{
public:
    template <typename HasherT>
    static typename HasherT::digest_type hash(const uint8_t* data, int size)
    {
        HasherT hasher;
        hasher.update(data, size);
        return hasher.finalize();
    }
    template <typename HasherT>
    static typename HasherT::digest_type hash(const char* data, int size)
    {
        return hashing::hash<HasherT>(reinterpret_cast<const uint8_t*>(data), size);
    }

    template <typename HasherT, typename ...ARGS>
    static typename HasherT::digest_type hash_data(bool bigEndian, const ARGS&... args)
    {
        hash_stream<HasherT> hstream;
        data_output_stream os(&hstream, bigEndian);
        return do_hash(hstream, os, args...);
    }

private:
    template <typename HasherStreamT, typename T>
    static typename HasherStreamT::digest_type do_hash(HasherStreamT& hstream, data_output_stream& os, const T& arg)
    {
        os << arg;
        return hstream.finalize();
    }
    template <typename HasherStreamT, typename T, typename ...ARGS>
    static typename HasherStreamT::digest_type do_hash(HasherStreamT& hstream, data_output_stream& os, const T& arg, const ARGS&... args)
    {
        os << arg;
        return do_hash(hstream, os, args...);
    }
};


}
