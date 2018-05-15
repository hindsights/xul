#pragma once

#include <xul/io/input_stream.hpp>
#include <xul/lang/object_impl.hpp>

#include <vector>
#include <string>
#include <istream>
#include <stdexcept>
#include <assert.h>
#include <stdint.h>
#include <limits.h>


namespace xul {


class istream_stream : public object_impl<input_stream>
{
public:
    explicit istream_stream(std::istream& is) : m_is(is)
    {
        int64_t pos = m_is.tellg();
        m_is.seekg(0, std::ios_base::end);
        m_size = m_is.tellg();
        m_is.seekg(pos);
    }


    virtual size_t read(uint8_t* buf, size_t size)
    {
        m_is.read(reinterpret_cast<char*>(buf), size);
        return size;
    }

    virtual int read_byte()
    {
        return m_is.get();
    }
    virtual int peek_byte()
    {
        return m_is.peek();
    }

    virtual int64_t available() const
    {
        int64_t pos = m_is.tellg();
        assert(pos <= m_size);
        return m_size - pos;
    }

    virtual bool skip( int64_t size )
    {
        m_is.seekg(size, std::ios_base::cur);
        return m_is.good();
    }

    //virtual bool unread(size_t size) = 0;

    virtual int64_t position() const
    {
        return m_is.tellg();
    }

    virtual bool seek(int64_t pos)
    {
        m_is.seekg(pos);
        return m_is.good();
    }

    virtual int64_t size() const
    {
        return m_size;
    }

private:
    std::istream& m_is;
    int64_t m_size;
};


}
