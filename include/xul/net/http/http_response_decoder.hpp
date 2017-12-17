#pragma once

#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/net/url_response.hpp>
#include <xul/net/http/http_response_parser.hpp>
#include <xul/log/log.hpp>


namespace xul {


class http_response_extractor
{
public:
    explicit http_response_extractor(int maxBufferSize = 64 * 1024) : m_max_buffer_size(maxBufferSize)
    {
        reset();
    }

    bool is_header_finished() const
    {
        return m_header_finished;
    }

    void reset()
    {
        XUL_APP_DEBUG("http_response_extractor.reset");
        m_header_finished = false;
        m_response = create_url_response();
        m_buffer.resize(0);
    }
    int feed(const uint8_t* data, size_t size)
    {
        if (m_header_finished)
        {
            assert(false);
            return 0;
        }
        int previous_buffer_size = m_buffer.size();
        int header_size = 0;
        if (m_buffer.empty())
        {
            header_size = try_parse(data, size);
        }
        else
        {
            if (m_buffer.size() + size > m_max_buffer_size)
            {
                assert(false);
                return -1;
            }
            m_buffer.append(data, size);
            byte_buffer temp_buf;
            temp_buf.swap(m_buffer);
            header_size = try_parse(temp_buf.data(), temp_buf.size());
        }
        if (header_size < 0)
        {
            assert(false);
            return -1;
        }
        if (0 == header_size)
            return size;
        int consumed_size = header_size - previous_buffer_size;
        assert(consumed_size > 0);
        return consumed_size;
    }
    url_response* get_response()
    {
        return m_response.get();
    }

private:
    int try_parse(const uint8_t* data, size_t size)
    {
        assert(!m_header_finished);
        // header data is fed into line_decoder to parse header
        const char* bufstart = reinterpret_cast<const char*>(data);
        const char* pos = http_message::find_header_end(reinterpret_cast<const char*>(data), size);
        if (NULL == pos)
        {
            //assert(size < 4);
            m_buffer.assign(data, size);
            return 0;
        }
        int headerSize = pos - bufstart + 4;
        http_response_parser parser(m_response.get());
        if (false == parser.parse(bufstart, headerSize))
        {
            assert(false);
            return -1;
        }
        m_header_finished = true;
        return headerSize;
    }

private:
    byte_buffer m_buffer;
    const int m_max_buffer_size;
    bool m_header_finished;
    boost::intrusive_ptr<url_response> m_response;
};


class http_response_decoder : public message_decoder_adapter
{
public:
    class decoder_message : public decoder_message_base
    {
    public:
        url_response* response;
        const uint8_t* data;
        int size;

        explicit decoder_message(url_response* resp) : response(resp), data(NULL), size(0)
        { }
        explicit decoder_message(const uint8_t* _data, int _size) : response(NULL), data(_data), size(_size) { }
    };

    explicit http_response_decoder(int maxBufferSize = 8 * 1024) : m_max_buffer_size(maxBufferSize)
    {
        m_receiving_header = true;
        m_abort = false;
    }

    virtual bool feed(const uint8_t* data, int size)
    {
        if (m_buffer.empty())
        {
            do_decode(data, size);
        }
        else
        {
            if (m_buffer.size() + size > m_max_buffer_size)
                return false;
            m_buffer.append(data, size);
            byte_buffer temp_buf;
            temp_buf.swap(m_buffer);
            do_decode(temp_buf.data(), temp_buf.size());
        }
        return true;
    }
    virtual void abort()
    {
        m_abort = true;
    }
    virtual bool is_aborted() const { return m_abort; }
    virtual const uint8_t* get_remaining_data() const { return m_buffer.data(); }
    virtual int get_remaining_size() const { return m_buffer.size(); }
    virtual void reset()
    {
        XUL_APP_DEBUG("http_response_decoder.reset " << make_tuple(m_receiving_header, 0));
        m_abort = false;
        m_receiving_header = true;
        m_buffer.clear();
    }

protected:
    int do_decode(const uint8_t* data, int size)
    {
        //XUL_APP_DEBUG("do_decode " << size);
        if (m_receiving_header)
        {
            // header data is fed into line_decoder to parse header
            const char* bufstart = reinterpret_cast<const char*>(data);
            const char* pos = http_message::find_header_end(reinterpret_cast<const char*>(data), size);
            if (NULL == pos)
            {
                //assert(size < 4);
                m_buffer.assign(data, size);
                return 0;
            }
            int headerSize = pos - bufstart + 4;
            boost::intrusive_ptr<url_response> req = create_url_response();
            http_response_parser parser(req.get());
            if (false == parser.parse(bufstart, headerSize))
            {
                this->abort();
                do_get_listener()->on_decoder_error(this, -1);
                return -1;
            }
            m_receiving_header = false;
            decoder_message headermsg(parser.get_response());
            do_get_listener()->on_decoder_message(this, &headermsg);
            int remaining_size = size - headerSize;
            if (remaining_size > 0)
            {
                decoder_message body_msg(reinterpret_cast<const uint8_t*>(pos) + 4, size - headerSize);
                do_get_listener()->on_decoder_message(this, &body_msg);
            }
            return headerSize;
        }
        decoder_message msg(data, size);
        do_get_listener()->on_decoder_message(this, &msg);
        return size;
    }

private:
    byte_buffer m_buffer;
    const int m_max_buffer_size;
    bool m_abort;
    bool m_receiving_header;
};


}
