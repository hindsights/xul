#pragma once

#include <xul/lang/object.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/data/bit_converter.hpp>
#include <xul/data/buffer.hpp>
#include <vector>


namespace xul {


class message_packet_pipe : public object_impl<object>
{
public:
    explicit message_packet_pipe(bool isBigEndian, int maxMsgSize = 4 * 1024 * 1024)
        : m_max_message_length(maxMsgSize)
        , m_packet_length_converter(false)
        , m_position(0)
    {
        set_big_endian(isBigEndian);
    }
    virtual ~message_packet_pipe()
    {
    }

    void set_big_endian(bool isBigEndian)
    {
        m_packet_length_converter.set_big_endian(isBigEndian);
    }

    void feed(const uint8_t* data, int size)
    {
        check_buffer();
        m_buffer.insert(m_buffer.end(), data, data + size);
    }

    const uint8_t* read(int* size)
    {
        assert(size);
        if (m_position + 4 > m_buffer.size())
            return NULL;
        uint32_t msgLen = m_packet_length_converter.to_dword(&m_buffer[m_position]);
        if (msgLen > m_max_message_length)
        {
            return NULL;
        }
        int dataPos = m_position + 4;
        if (msgLen == 0)
        {
            *size = 0;
            return NULL;
        }
        if (dataPos + msgLen > m_buffer.size())
            return NULL;
        m_position += (4 + msgLen);
        *size = msgLen;
        return &m_buffer[dataPos];
    }

    int size() const
    {
        return m_buffer.size();
    }
    int position() const
    {
        return m_position;
    }
    int available() const
    {
        return size() - position();
    }

private:
    void check_buffer()
    {
        if (m_position > 0)
        {
            if (m_buffer.size() >= m_position)
                m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_position);
            m_position = 0;
        }
    }

protected:
    std::vector<uint8_t> m_buffer;
    int m_position;
    const int m_max_message_length;

    bit_converter m_packet_length_converter;
};


}
