#pragma once

#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/data/bit_converter.hpp>


namespace xul {


class message_packet_decoder : public buffered_message_decoder
{
public:
    typedef decoder_buffer_message decoder_message;

    explicit message_packet_decoder(bool isBigEndian, int maxMsgSize = 4 * 1024 * 1024)
        : buffered_message_decoder(maxMsgSize + 10)
        , m_max_message_length(maxMsgSize)
        , m_packet_length_converter(false)
    {
        set_big_endian(isBigEndian);
    }
    virtual ~message_packet_decoder()
    {
    }

    void set_big_endian(bool isBigEndian)
    {
        m_packet_length_converter.set_big_endian(isBigEndian);
    }

protected:
    virtual bool do_decode()
    {
        if (m_position + 4 > m_buffer.size())
            return false;
        uint32_t msgLen = m_packet_length_converter.to_dword(&m_buffer[m_position]);
        if (msgLen > m_max_message_length)
        {
            this->abort();
            get_listener()->on_decoder_error(this, 3);
            return false;
        }
        int dataPos = m_position + 4;
        if (msgLen == 0)
        {
            decoder_message msg(NULL, 0);
            get_listener()->on_decoder_message(this, &msg);
            return true;
        }
        if (dataPos + msgLen > m_buffer.size())
            return false;
        decoder_message msg(&m_buffer[dataPos], msgLen);
        get_listener()->on_decoder_message(this, &msg);
        m_position = dataPos + msgLen;
        return true;
    }

protected:
    const int m_max_message_length;

    bit_converter m_packet_length_converter;
};


}
