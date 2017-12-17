#pragma once

#include <xul/io/codec/buffered_data_codec.hpp>
#include <stdint.h>


namespace xul {


class chunked_data_decoder : public buffered_data_codec
{
public:
    explicit chunked_data_decoder()
        : buffered_data_codec(128*1024)
        , m_last_chunk_size(-1)
        , m_current_chunk_size(-1)
        , m_received_chunk_size(-1)
    {
    }
    virtual ~chunked_data_decoder()
    {

    }

    virtual int flush(data_codec_callback* callback)
    {
        return -1;
    }

    virtual void reset()
    {
        m_last_chunk_size = -1;
        m_current_chunk_size = -1;
        m_received_chunk_size = -1;
    }

protected:
    int decode_once(const uint8_t* data, int size, data_codec_callback* callback)
    {
        if (0 == m_current_chunk_size)
        {
            assert(false);
            return -1;
        }
        if (m_current_chunk_size > 0)
        {
            return decode_chunk_data(data, size, callback);
        }
        if (0 == m_last_chunk_size)
        {
            return decode_chunk_trailer(data, size, callback);
        }
        return decode_chunk_start(data, size, callback);
    }
    int decode_chunk_data(const uint8_t* data, int size, data_codec_callback* callback)
    {
        assert(m_received_chunk_size >= 0 && m_received_chunk_size < m_current_chunk_size);
        int remaining_size = m_current_chunk_size - m_received_chunk_size;
        int read_size = numerics::get_min(remaining_size, size);
        m_received_chunk_size += read_size;
        callback->on_data_codec_buffer(this, data, read_size);
        return read_size;
    }
    int decode_chunk_trailer(const uint8_t* data, int size, data_codec_callback* callback)
    {
        if (size < 2)
        {
            return 0;
        }
        if (data[0] == '\r' && data[1] == '\n')
        {
            callback->on_data_codec_buffer(this, data, 0);
            return 2;
        }
        const uint8_t crln2[4] = { '\r', '\n', '\r', '\n' };
        const uint8_t* pos = std::search(data, data + size, crln2, crln2 + 4);
        if (pos == data + size)
        {
            return 0;
        }
        callback->on_data_codec_buffer(this, data, 0);
        return data + size - pos;
    }
    int decode_chunk_start(const uint8_t* data, int size, data_codec_callback* callback)
    {
        const uint8_t crln[2] = { '\r', '\n' };
        const uint8_t* pos = std::search(data, data + size, crln, crln + 2);
        if (pos == data + size)
        {
            return 0;
        }
        assert(pos - data + 2 < size);
        m_last_chunk_size = m_current_chunk_size;
        std::string chunklenstr(reinterpret_cast<const char*>(data), pos - data);
        m_current_chunk_size = xul::numerics::parse<int64_t>(chunklenstr, -1);
        if (m_current_chunk_size < 0)
            return -1;
        XUL_APP_DEBUG("decode_chunk_start " << chunklenstr);
        return 2;
    }

private:
    int64_t m_last_chunk_size;
    int64_t m_current_chunk_size;
    int64_t m_received_chunk_size;
};


}
