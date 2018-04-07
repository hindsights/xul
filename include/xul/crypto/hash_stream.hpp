#pragma once

#include <xul/io/output_stream_impl.hpp>


namespace xul {


template <typename HasherT>
class hash_stream : public output_stream_impl
{
public:
    std::string buffer;
    typedef HasherT hasher_type;
    typedef typename hasher_type::digest_type digest_type;
    static const int digest_size = digest_type::byte_size;

    hash_stream()
    {
    }
    void reset()
    {
        m_hasher.reset();
    }

    digest_type finalize()
    {
        return m_hasher.finalize();
    }

protected:
    virtual void do_write_bytes(const uint8_t* data, int size)
    {
        m_hasher.update(data, size);
        buffer.append(reinterpret_cast<const char*>(data), size);
    }
    virtual void do_write_byte(uint8_t val)
    {
        m_hasher.update(&val, 1);
        buffer.append(1, val);
    }

private:
    HasherT m_hasher;
};


}
