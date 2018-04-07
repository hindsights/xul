#pragma once

#include <xul/io/data_output_stream.hpp>
#include <xul/crypto/hash_stream.hpp>
#include <xul/text/hex_encoding.hpp>
//#include <xul/log/log.hpp>

namespace xul {


class hashing
{
public:
    template <typename HasherT>
    static typename HasherT::digest_type hash(const void* data, int size)
    {
        HasherT hasher;
        hasher.update(data, size);
        return hasher.finalize();
    }
    template <typename HasherT, typename T>
    static typename HasherT::digest_type hash_object(const T& obj, bool bigEndian)
    {
        hash_stream<HasherT> hstream;
        data_output_stream os(&hstream, bigEndian);
        os << obj;
        std::string bufstr = xul::hex_encoding::lower_case().encode(hstream.buffer);
//        XUL_APP_DEBUG("hash_object " << hstream.buffer.size() << " " << bufstr);
        return hstream.finalize();
    }
};


}
