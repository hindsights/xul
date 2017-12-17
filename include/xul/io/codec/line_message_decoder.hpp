#pragma once

#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/io/codec/decoder_buffer_message.hpp>
#include <string>


namespace xul {


class line_message_decoder : public buffered_message_decoder
{
public:
    typedef decoder_buffer_message decoder_message;

    explicit line_message_decoder(const std::string& delimiter = "\r\n", int maxBufferSize = 8 * 1024)
        : buffered_message_decoder(maxBufferSize)
        , m_delimiter(delimiter)
    {
    }
    virtual ~line_message_decoder()
    {
    }

protected:
    virtual int do_decode(const uint8_t* data, int size)
    {
        const uint8_t* pos = std::search(data, data + size, m_delimiter.begin(), m_delimiter.end());
        if (pos == data + size)
        {
            return 0;
        }
        pos += m_delimiter.size();
        assert(pos <= data + size);
        assert(pos > data);
        int offset = pos - data;
        assert(offset > 0);
        decoder_message msg(data, offset);
        do_get_listener()->on_decoder_message(this, &msg);
        return offset;
    }

protected:
    std::string m_delimiter;
};


}
