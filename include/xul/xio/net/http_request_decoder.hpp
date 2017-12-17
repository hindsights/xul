#pragma once

#include <xul/io/codec/buffered_message_decoder.hpp>
#include <xul/net/url_request.hpp>
#include <xul/net/http/http_request_parser.hpp>
#include <xul/os/paths.hpp>


namespace xul {


class http_request_decoder : public buffered_message_decoder
{
public:
    class decoder_message : public decoder_message_base
    {
    public:
        url_request* request;

        const uint8_t* data;
        int size;

        explicit decoder_message(url_request* req) : request(req), data(NULL), size(0) { }
        explicit decoder_message(const uint8_t* _data, int _size) : request(NULL), data(_data), size(_size) { }
    };

    http_request_decoder()
    {
        m_receiving_header = true;
    }

    virtual void reset()
    {
        buffered_message_decoder::reset();
        m_receiving_header = true;
    }

protected:
    virtual int do_decode(const uint8_t* data, int size)
    {
        if (m_receiving_header)
        {
            // header data is fed into line_decoder to parse header
            const char* bufstart = reinterpret_cast<const char*>(data);
            const char* pos = http_message::find_header_end(reinterpret_cast<const char*>(data), size);
            if (NULL == pos)
                return 0;
            int headerSize = pos - bufstart + 4;
            boost::intrusive_ptr<url_request> request = create_url_request();
            http_request_parser parser(request.get());
            if (false == parser.parse(bufstart, headerSize))
            {
                this->abort();
                do_get_listener()->on_decoder_error(this, -1);
                return -1;
            }
            decoder_message msg(parser.get_request());
            do_get_listener()->on_decoder_message(this, &msg);
            m_receiving_header = false;
            return headerSize;
        }
        decoder_message msg(data, size);
        do_get_listener()->on_decoder_message(this, &msg);
        return size;
    }

private:
    bool m_receiving_header;
};


}
