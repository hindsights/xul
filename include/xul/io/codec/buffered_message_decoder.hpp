#pragma once

#include <xul/io/codec/message_decoder_adapter.hpp>
#include <xul/data/buffer.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <stddef.h>


namespace xul {


class buffered_message_decoder : public message_decoder_adapter
{
public:
    explicit buffered_message_decoder(int maxBufferSize = 8 * 1024)
        : m_max_buffer_size(maxBufferSize)
        , m_position(0)
        , m_abort(false)
    {
    }
    virtual ~buffered_message_decoder()
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

    virtual bool feed(const uint8_t* data, int size)
    {
        assert(m_position <= m_buffer.size());
        assert(m_buffer.size() <= m_max_buffer_size);

        check_buffer();
        if (m_buffer.empty())
        {
            // decode data from (data, size) directly
            int processedSize = decode_buffer(data, size);
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
                this->abort();
                do_get_listener()->on_decoder_error(this, 1);
                return true;
            }
            // append all the data into m_buffer
            m_buffer.insert(m_buffer.end(), data, data + size);
        }
        if (m_buffer.empty())
            return true;

        // decode data from m_buffer
        int processedSize = decode_buffer(&m_buffer[0], m_buffer.size());
        if (processedSize < 0)
            return true;
        //assert(m_buffer.size() >= processedSize);
        m_position += processedSize;
        check_buffer();
        return m_buffer.empty();
    }

    virtual void reset()
    {
        m_buffer.resize(0);
        m_position = 0;
        m_abort = false;
        m_failed = false;
    }

    virtual void abort()
    {
        reset();
        m_abort = true;
    }
    virtual bool is_aborted() const
    {
        return m_abort;
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
    virtual int do_decode(const uint8_t* data, int size) = 0;

    int decode_buffer(const uint8_t* data, int size)
    {
        const uint8_t* pos = data;
        int len = size;
        int processedSize = 0;
        while (processedSize < size)
        {
            if (m_abort)
                break;
            int ret = do_decode(pos, len);
            if (ret < 0)
            {
                this->abort();
                do_get_listener()->on_decoder_error(this, 1);
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
    bool m_abort;
    bool m_failed;
};


}
