#pragma once

#include <xul/io/codec/data_codec.hpp>
#include <xul/data/buffer.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <stddef.h>
#include <stdint.h>


namespace xul {


class buffered_data_codec : public object_impl<data_codec>
{
public:
    explicit buffered_data_codec(int maxBufferSize = 8 * 1024)
        : m_max_buffer_size(maxBufferSize)
        , m_position(0)
    {
    }
    virtual ~buffered_data_codec()
    {

    }

    void set_max_buffer_size(int maxbufsize)
    {
        if (maxbufsize < 1)
        {
            assert(false);
            return;
        }
        m_max_buffer_size = maxbufsize;
    }

    virtual int feed(const uint8_t* data, int size, data_codec_callback* callback)
    {
        assert(m_position <= m_buffer.size());
        assert(m_buffer.size() <= m_max_buffer_size);

        check_buffer();
        if (m_buffer.empty())
        {
            // decode data from (data, size) directly
            int processedSize = decode_buffer(data, size, callback);
            if (processedSize < 0)
                return true;
            if (size > processedSize)
            {
                // save the remaining data into m_buffer
                m_buffer.insert(m_buffer.end(), data + processedSize, data + size);
            }
        }
        else
        {
            if (m_buffer.size() + size > m_max_buffer_size)
            {
                return -1;
            }
            // append all the data into m_buffer
            m_buffer.insert(m_buffer.end(), data, data + size);
        }
        if (m_buffer.empty())
            return true;

        // decode data from m_buffer
        int processedSize = decode_buffer(&m_buffer[0], m_buffer.size(), callback);
        if (processedSize < 0)
            return true;
        //assert(m_buffer.size() >= processedSize);
        m_position += processedSize;
        check_buffer();
        return m_buffer.empty();
    }

    //virtual bool flush(data_codec_callback* callback);

    virtual void reset()
    {
        m_buffer.resize(0);
        m_position = 0;
    }

    virtual const uint8_t* get_remaining_data() const
    {
        if (m_position >= m_buffer.size())
            return NULL;
        return &m_buffer[m_position];
    }
    virtual int get_remaining_size() const
    {
        assert(m_position <= m_buffer.size());
        return m_buffer.size() - m_position;
    }

protected:
    virtual int decode_once(const uint8_t* data, int size, data_codec_callback* callback) = 0;

    int decode_buffer(const uint8_t* data, int size, data_codec_callback* callback)
    {
        const uint8_t* pos = data;
        int len = size;
        int processedSize = 0;
        while (processedSize < size)
        {
            int ret = decode_once(pos, len, callback);
            if (ret < 0)
            {
                return -1;
            }
            if (0 == ret)
            {
                break;
            }
            pos += ret;
            processedSize += ret;
            len -= ret;
        }
        return processedSize;
    }

    void check_buffer()
    {
        if (m_position > 0)
        {
            if (m_buffer.size() >= m_position)
                m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_position);
            m_position = 0;
        }
    }

private:
    std::vector<uint8_t> m_buffer;
    int m_position;
    int m_max_buffer_size;
};


}
