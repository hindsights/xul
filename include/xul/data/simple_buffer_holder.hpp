#pragma once

#include <xul/data/buffer.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class simple_buffer_holder : public object_impl<buffer_holder>
{
public:
    simple_buffer_holder()
    {
    }
    explicit simple_buffer_holder(const uint8_t* data, size_t size)
        : m_buffer(data, size)
    {
    }
    virtual const uint8_t* get_data() const
    {
        return m_buffer.data();
    }
    virtual uint8_t* get_buffer()
    {
        return m_buffer.data();
    }
    virtual size_t get_size() const
    {
        return m_buffer.size();
    }

private:
    byte_buffer m_buffer;
};


}
